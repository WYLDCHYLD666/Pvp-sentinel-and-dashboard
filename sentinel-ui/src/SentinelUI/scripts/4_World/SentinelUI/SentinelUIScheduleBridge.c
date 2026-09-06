class SentinelUIScheduleProfile
{
    string id;
    string name;
    string pvp_mode;
    bool base_raiding;
    bool vehicle_damage;
    bool building_damage;
    bool punishments;
}

class SentinelUIScheduleTransition
{
    string at;
    string type;
    string name;
    string profile;
}

class SentinelUIScheduleConfig
{
    bool enabled;
    ref array<ref SentinelUIScheduleProfile> profiles;
}

class SentinelUIScheduleStateData
{
    string now;
    bool enabled;
    string active_event;
    ref SentinelUIScheduleProfile active_profile;
    string active_since;
    string active_until;
    ref SentinelUIScheduleTransition next_transition;
}

class SentinelUISchedulePayload
{
    ref SentinelUIScheduleConfig config;
    ref SentinelUIScheduleStateData state;
}

class SentinelUIScheduleResponse
{
    bool ok;
    ref SentinelUISchedulePayload data;
}

class SentinelUIScheduleRestCallback : RestCallback
{
    override void OnSuccess(string data, int dataSize)
    {
        SentinelUIScheduleBridge.GetInstance().OnScheduleResponse(data);
    }

    override void OnError(int errorCode)
    {
        SentinelUIScheduleBridge.GetInstance().OnScheduleRequestFailed("HTTP error " + errorCode.ToString());
    }

    override void OnTimeout()
    {
        SentinelUIScheduleBridge.GetInstance().OnScheduleRequestFailed("request timed out");
    }
}

class SentinelUIScheduleBridge
{
    static const string API_ROOT = "https://sentineladmin.co.uk/";
    static const string API_PATH = "dashboard/api/schedule.php";
    static const int POLL_INTERVAL_MS = 10000;
    static const int COUNTDOWN_TICK_MS = 250;
    static const int PURGE_COUNTDOWN_SECONDS = 10;

    protected static ref SentinelUIScheduleBridge s_Instance;
    protected ref SentinelUIScheduleRestCallback m_Callback;
    protected RestContext m_Context;
    protected bool m_Initialized;
    protected bool m_RequestPending;
    protected bool m_HasState;
    protected bool m_Active;
    protected string m_EventName = "Purge";
    protected string m_NextPurgeAt;
    protected string m_NextPurgeName = "Purge";
    protected int m_NextPurgeEpoch;
    protected int m_LastCountdown = -1;
    protected string m_FiredTransitionAt;
    protected int m_ConsecutiveFailures;

    static SentinelUIScheduleBridge GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SentinelUIScheduleBridge();
        }

        return s_Instance;
    }

    void Init()
    {
        if (m_Initialized || !GetGame() || !GetGame().IsServer())
        {
            return;
        }

        RestApi api = GetRestApi();

        if (!api)
        {
            api = CreateRestApi();
        }

        if (!api)
        {
            Print("[SentinelUI] Purge scheduler bridge could not create REST API");
            return;
        }

        m_Context = api.GetRestContext(API_ROOT);
        m_Callback = new SentinelUIScheduleRestCallback();
        m_Initialized = true;

        PollNow();
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SentinelUIScheduleBridge.PollTick, POLL_INTERVAL_MS, true);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SentinelUIScheduleBridge.CountdownTick, COUNTDOWN_TICK_MS, true);
        Print("[SentinelUI] Purge scheduler bridge started endpoint=" + API_ROOT + API_PATH);
    }

    static void PollTick()
    {
        GetInstance().PollNow();
    }

    static void CountdownTick()
    {
        GetInstance().UpdateCountdown();
    }

    protected void PollNow()
    {
        if (!m_Initialized || !m_Context || m_RequestPending)
        {
            return;
        }

        m_RequestPending = true;
        m_Context.GET(m_Callback, API_PATH);
    }

    void OnScheduleRequestFailed(string reason)
    {
        m_RequestPending = false;
        m_ConsecutiveFailures++;

        if (m_ConsecutiveFailures == 1 || (m_ConsecutiveFailures % 6) == 0)
        {
            Print("[SentinelUI] Purge scheduler poll failed: " + reason + " failures=" + m_ConsecutiveFailures.ToString());
        }
    }

    void OnScheduleResponse(string json)
    {
        m_RequestPending = false;

        SentinelUIScheduleResponse response = new SentinelUIScheduleResponse();
        string parseError;

        if (!JsonFileLoader<SentinelUIScheduleResponse>.LoadData(json, response, parseError))
        {
            OnScheduleRequestFailed(parseError);
            return;
        }

        if (!response || !response.ok || !response.data || !response.data.state)
        {
            OnScheduleRequestFailed("API returned no usable state");
            return;
        }

        m_ConsecutiveFailures = 0;

        SentinelUIScheduleStateData state = response.data.state;
        bool isPurge = state.enabled && IsPurgeProfile(state.active_profile);
        string currentEventName = state.active_event;

        if (currentEventName == "" && state.active_profile)
        {
            currentEventName = state.active_profile.name;
        }

        if (currentEventName == "")
        {
            currentEventName = "Purge";
        }

        UpdateNextPurgeTransition(response.data);

        if (!m_HasState)
        {
            m_HasState = true;
            m_Active = isPurge;
            m_EventName = currentEventName;
            int initialActiveValue;

            if (m_Active)
            {
                initialActiveValue = 1;
            }

            Broadcast(SentinelUIZoneProtocol.KIND_PURGE_SYNC, initialActiveValue, m_EventName);
            Print("[SentinelUI] Purge scheduler initial state active=" + m_Active.ToString() + " event=" + m_EventName);
            return;
        }

        if (!m_Active && isPurge)
        {
            m_Active = true;
            m_EventName = currentEventName;
            Broadcast(SentinelUIZoneProtocol.KIND_PURGE_START, 0, m_EventName);
            Print("[SentinelUI] Purge started from dashboard state event=" + m_EventName);
        }
        else if (m_Active && !isPurge)
        {
            m_Active = false;
            Broadcast(SentinelUIZoneProtocol.KIND_PURGE_END, 0, m_EventName);
            Print("[SentinelUI] Purge ended from dashboard state event=" + m_EventName);
            m_EventName = currentEventName;
        }
        else
        {
            m_EventName = currentEventName;
        }
    }

    protected bool IsPurgeProfile(SentinelUIScheduleProfile profile)
    {
        return profile && profile.pvp_mode == "WHOLE_MAP_KOS";
    }

    protected bool IsPurgeProfileId(SentinelUISchedulePayload payload, string profileId)
    {
        if (!payload || !payload.config || !payload.config.profiles || profileId == "")
        {
            return false;
        }

        foreach (SentinelUIScheduleProfile profile : payload.config.profiles)
        {
            if (profile && profile.id == profileId)
            {
                return IsPurgeProfile(profile);
            }
        }

        return false;
    }

    protected void UpdateNextPurgeTransition(SentinelUISchedulePayload payload)
    {
        string nextAt = "";
        string nextName = "Purge";
        int nextEpoch;

        if (payload && payload.state && payload.state.next_transition)
        {
            SentinelUIScheduleTransition transition = payload.state.next_transition;

            if (transition.type == "start" && IsPurgeProfileId(payload, transition.profile))
            {
                nextAt = transition.at;
                nextEpoch = ParseIso8601Epoch(nextAt);

                if (transition.name != "")
                {
                    nextName = transition.name;
                }
            }
        }

        if (nextAt != m_NextPurgeAt)
        {
            m_NextPurgeAt = nextAt;
            m_NextPurgeName = nextName;
            m_NextPurgeEpoch = nextEpoch;
            m_LastCountdown = -1;

            if (m_FiredTransitionAt != m_NextPurgeAt)
            {
                m_FiredTransitionAt = "";
            }

            if (m_NextPurgeEpoch > 0)
            {
                Print("[SentinelUI] next Purge transition=" + m_NextPurgeAt + " epoch=" + m_NextPurgeEpoch.ToString());
            }
        }
    }

    protected void UpdateCountdown()
    {
        if (!m_HasState || m_Active || m_NextPurgeEpoch <= 0)
        {
            return;
        }

        int remaining = m_NextPurgeEpoch - GetUtcEpochNow();

        if (remaining > PURGE_COUNTDOWN_SECONDS)
        {
            m_LastCountdown = -1;
            return;
        }

        if (remaining >= 1 && remaining <= PURGE_COUNTDOWN_SECONDS)
        {
            if (remaining != m_LastCountdown)
            {
                m_LastCountdown = remaining;
                Broadcast(SentinelUIZoneProtocol.KIND_PURGE_COUNTDOWN, remaining, m_NextPurgeName);
                Print("[SentinelUI] Purge countdown " + remaining.ToString());
            }

            return;
        }

        if (remaining >= -10 && m_FiredTransitionAt != m_NextPurgeAt)
        {
            m_FiredTransitionAt = m_NextPurgeAt;
            m_Active = true;
            m_EventName = m_NextPurgeName;
            m_LastCountdown = -1;
            Broadcast(SentinelUIZoneProtocol.KIND_PURGE_START, 0, m_EventName);
            Print("[SentinelUI] Purge started at scheduled transition event=" + m_EventName);
        }
    }

    void SendStateToPlayer(PlayerBase player)
    {
        if (!m_HasState || !player || !player.GetIdentity())
        {
            return;
        }

        int activeValue;

        if (m_Active)
        {
            activeValue = 1;
        }

        SendToPlayer(player, SentinelUIZoneProtocol.KIND_PURGE_SYNC, activeValue, m_EventName);
    }

    protected void Broadcast(int kind, int value, string eventName)
    {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);

            if (player && player.GetIdentity())
            {
                SendToPlayer(player, kind, value, eventName);
            }
        }
    }

    protected void SendToPlayer(PlayerBase player, int kind, int value, string eventName)
    {
        Param3<int, int, string> payload = new Param3<int, int, string>(kind, value, eventName);
        GetGame().RPCSingleParam(player, SentinelUIZoneProtocol.RPC_PURGE_STATE, payload, true, player.GetIdentity());
    }

    protected int GetUtcEpochNow()
    {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        GetYearMonthDayUTC(year, month, day);
        GetHourMinuteSecondUTC(hour, minute, second);
        return ToUnixEpoch(year, month, day, hour, minute, second);
    }

    protected int ParseIso8601Epoch(string iso)
    {
        if (iso.Length() < 19)
        {
            return 0;
        }

        int year = iso.Substring(0, 4).ToInt();
        int month = iso.Substring(5, 2).ToInt();
        int day = iso.Substring(8, 2).ToInt();
        int hour = iso.Substring(11, 2).ToInt();
        int minute = iso.Substring(14, 2).ToInt();
        int second = iso.Substring(17, 2).ToInt();
        int offsetSeconds;

        if (iso.Length() >= 25)
        {
            string offsetSign = iso.Substring(19, 1);
            int offsetHours = iso.Substring(20, 2).ToInt();
            int offsetMinutes = iso.Substring(23, 2).ToInt();
            offsetSeconds = (offsetHours * 3600) + (offsetMinutes * 60);

            if (offsetSign == "-")
            {
                offsetSeconds = -offsetSeconds;
            }
        }

        return ToUnixEpoch(year, month, day, hour, minute, second) - offsetSeconds;
    }

    protected int ToUnixEpoch(int year, int month, int day, int hour, int minute, int second)
    {
        int days;

        for (int y = 1970; y < year; y++)
        {
            if (IsLeapYear(y))
            {
                days += 366;
            }
            else
            {
                days += 365;
            }
        }

        for (int m = 1; m < month; m++)
        {
            days += DaysInMonth(year, m);
        }

        days += day - 1;
        return (days * 86400) + (hour * 3600) + (minute * 60) + second;
    }

    protected bool IsLeapYear(int year)
    {
        if ((year % 400) == 0)
        {
            return true;
        }

        if ((year % 100) == 0)
        {
            return false;
        }

        return (year % 4) == 0;
    }

    protected int DaysInMonth(int year, int month)
    {
        if (month == 2)
        {
            if (IsLeapYear(year))
            {
                return 29;
            }

            return 28;
        }

        if (month == 4 || month == 6 || month == 9 || month == 11)
        {
            return 30;
        }

        return 31;
    }
}
