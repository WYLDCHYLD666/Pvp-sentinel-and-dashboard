class SentinelEventStore
{
    private static ref map<string, string> s_OnlinePlayers = new map<string, string>();
    private static ref array<int> s_EventSequences = new array<int>();
    private static ref array<string> s_EventTimestamps = new array<string>();
    private static ref array<string> s_EventTypes = new array<string>();
    private static ref array<string> s_EventPlayerNames = new array<string>();
    private static ref array<string> s_EventPlayerIds = new array<string>();
    private static ref array<string> s_EventKillerNames = new array<string>();
    private static ref array<string> s_EventKillerIds = new array<string>();
    private static ref array<string> s_EventWeaponClasses = new array<string>();
    private static ref array<string> s_EventWeaponDisplayNames = new array<string>();
    private static ref array<string> s_EventCauses = new array<string>();
    private static ref array<float> s_EventDistances = new array<float>();
    private static ref array<vector> s_EventKillerPositions = new array<vector>();
    private static ref array<vector> s_EventVictimPositions = new array<vector>();
    private static bool s_HasBaseline = false;
    private static int s_NextSequence = 1;
    private static string s_SessionId = "";

    static void Init()
    {
        s_OnlinePlayers.Clear();
        s_EventSequences.Clear();
        s_EventTimestamps.Clear();
        s_EventTypes.Clear();
        s_EventPlayerNames.Clear();
        s_EventPlayerIds.Clear();
        s_EventKillerNames.Clear();
        s_EventKillerIds.Clear();
        s_EventWeaponClasses.Clear();
        s_EventWeaponDisplayNames.Clear();
        s_EventCauses.Clear();
        s_EventDistances.Clear();
        s_EventKillerPositions.Clear();
        s_EventVictimPositions.Clear();
        s_HasBaseline = false;
        s_NextSequence = 1;
        s_SessionId = BuildUtcTimestamp();

        WriteEventsFile();
        Print(SentinelConstants.LOG_PREFIX + " event store initialized session=" + s_SessionId);
    }

    static string GetSessionId()
    {
        return s_SessionId;
    }

    static void ScanPlayerLifecycle(array<Man> players)
    {
        ref map<string, string> currentPlayers = new map<string, string>();

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);

            if (!player)
            {
                continue;
            }

            PlayerIdentity identity = player.GetIdentity();

            if (!identity)
            {
                continue;
            }

            string playerId = identity.GetId();
            string playerName = identity.GetName();

            if (playerId == "")
            {
                continue;
            }

            currentPlayers.Insert(playerId, playerName);

            if (s_HasBaseline && !s_OnlinePlayers.Contains(playerId))
            {
                AddEvent("player_connected", playerName, playerId);
            }
        }

        if (s_HasBaseline)
        {
            foreach (string knownId, string knownName : s_OnlinePlayers)
            {
                if (!currentPlayers.Contains(knownId))
                {
                    AddEvent("player_disconnected", knownName, knownId);
                }
            }
        }

        s_OnlinePlayers.Clear();

        foreach (string currentId, string currentName : currentPlayers)
        {
            s_OnlinePlayers.Insert(currentId, currentName);
        }

        s_HasBaseline = true;
    }

    static void RecordPlayerDeathSnapshot(string victimName, string victimId, string killerName, string killerId, string weaponClass = "", string weaponDisplayName = "", string cause = "unknown", float distanceMetres = 0.0, vector killerPosition = "0 0 0", vector victimPosition = "0 0 0")
    {
        if (victimId == "")
        {
            Print(SentinelConstants.LOG_PREFIX + " death snapshot skipped: empty victim id; victim_name=" + victimName + " killer_id=" + killerId);
            return;
        }

        if (killerId != "" && killerId != victimId)
        {
            Print(SentinelConstants.LOG_PREFIX + " death classified as player_killed cause=" + cause + " weapon=" + weaponClass + " distance_m=" + distanceMetres.ToString());
            AddKillEvent(victimName, victimId, killerName, killerId, weaponClass, weaponDisplayName, cause, distanceMetres, killerPosition, victimPosition);
            return;
        }

        if (killerId == victimId && killerId != "")
        {
            Print(SentinelConstants.LOG_PREFIX + " death classified as player_died (self kill)");
        }
        else
        {
            Print(SentinelConstants.LOG_PREFIX + " death classified as player_died (no human killer identity)");
        }

        AddDeathEvent(victimName, victimId, cause, victimPosition);
    }

    static void RecordPlayerDeath(PlayerBase victim, Object killerObject)
    {
        if (!victim)
        {
            return;
        }

        PlayerBase killer = PlayerBase.Cast(killerObject);

        if (!killer || killer == victim)
        {
            RecordPlayerDied(victim);
            return;
        }

        RecordPlayerKilled(victim, killer);
    }

    static void RecordPlayerDied(PlayerBase player)
    {
        if (!player)
        {
            return;
        }

        PlayerIdentity identity = player.GetIdentity();

        if (!identity)
        {
            Print(SentinelConstants.LOG_PREFIX + " player_died skipped: missing identity");
            return;
        }

        string playerId = identity.GetId();
        string playerName = identity.GetName();

        if (playerId == "")
        {
            Print(SentinelConstants.LOG_PREFIX + " player_died skipped: empty player id");
            return;
        }

        AddDeathEvent(playerName, playerId, "unknown", player.GetPosition());
    }

    static void RecordPlayerKilled(PlayerBase victim, PlayerBase killer)
    {
        if (!victim || !killer)
        {
            RecordPlayerDied(victim);
            return;
        }

        PlayerIdentity victimIdentity = victim.GetIdentity();
        PlayerIdentity killerIdentity = killer.GetIdentity();

        if (!victimIdentity || !killerIdentity)
        {
            Print(SentinelConstants.LOG_PREFIX + " player_killed fallback: missing victim or killer identity");
            RecordPlayerDied(victim);
            return;
        }

        string victimId = victimIdentity.GetId();
        string victimName = victimIdentity.GetName();
        string killerId = killerIdentity.GetId();
        string killerName = killerIdentity.GetName();

        if (victimId == "" || killerId == "")
        {
            Print(SentinelConstants.LOG_PREFIX + " player_killed fallback: empty victim or killer id");
            RecordPlayerDied(victim);
            return;
        }

        vector killerPosition = killer.GetPosition();
        vector victimPosition = victim.GetPosition();
        float distanceMetres = vector.Distance(killerPosition, victimPosition);

        AddKillEvent(victimName, victimId, killerName, killerId, "", "", "player", distanceMetres, killerPosition, victimPosition);
    }

    private static void AddEvent(string eventType, string playerName, string playerId)
    {
        s_EventSequences.Insert(s_NextSequence);
        s_EventTimestamps.Insert(BuildUtcTimestamp());
        s_EventTypes.Insert(eventType);
        s_EventPlayerNames.Insert(playerName);
        s_EventPlayerIds.Insert(playerId);
        s_EventKillerNames.Insert("");
        s_EventKillerIds.Insert("");
        s_EventWeaponClasses.Insert("");
        s_EventWeaponDisplayNames.Insert("");
        s_EventCauses.Insert("");
        s_EventDistances.Insert(0.0);
        s_EventKillerPositions.Insert("0 0 0");
        s_EventVictimPositions.Insert("0 0 0");
        s_NextSequence++;

        EnforceRetentionLimit();
        WriteEventsFile();

        Print(SentinelConstants.LOG_PREFIX + " " + eventType + " = " + playerName + " (" + playerId + ")");
    }

    private static void AddDeathEvent(string victimName, string victimId, string cause, vector victimPosition)
    {
        s_EventSequences.Insert(s_NextSequence);
        s_EventTimestamps.Insert(BuildUtcTimestamp());
        s_EventTypes.Insert("player_died");
        s_EventPlayerNames.Insert(victimName);
        s_EventPlayerIds.Insert(victimId);
        s_EventKillerNames.Insert("");
        s_EventKillerIds.Insert("");
        s_EventWeaponClasses.Insert("");
        s_EventWeaponDisplayNames.Insert("");
        s_EventCauses.Insert(cause);
        s_EventDistances.Insert(0.0);
        s_EventKillerPositions.Insert("0 0 0");
        s_EventVictimPositions.Insert(victimPosition);
        s_NextSequence++;

        EnforceRetentionLimit();
        WriteEventsFile();

        Print(SentinelConstants.LOG_PREFIX + " player_died = " + victimName + " (" + victimId + ") cause=" + cause);
    }

    private static void AddKillEvent(string victimName, string victimId, string killerName, string killerId, string weaponClass, string weaponDisplayName, string cause, float distanceMetres, vector killerPosition, vector victimPosition)
    {
        s_EventSequences.Insert(s_NextSequence);
        s_EventTimestamps.Insert(BuildUtcTimestamp());
        s_EventTypes.Insert("player_killed");
        s_EventPlayerNames.Insert(victimName);
        s_EventPlayerIds.Insert(victimId);
        s_EventKillerNames.Insert(killerName);
        s_EventKillerIds.Insert(killerId);
        s_EventWeaponClasses.Insert(weaponClass);
        s_EventWeaponDisplayNames.Insert(weaponDisplayName);
        s_EventCauses.Insert(cause);
        s_EventDistances.Insert(distanceMetres);
        s_EventKillerPositions.Insert(killerPosition);
        s_EventVictimPositions.Insert(victimPosition);
        s_NextSequence++;

        EnforceRetentionLimit();
        WriteEventsFile();

        Print(SentinelConstants.LOG_PREFIX + " player_killed = " + killerName + " (" + killerId + ") -> " + victimName + " (" + victimId + ") weapon=" + weaponClass + " distance_m=" + distanceMetres.ToString());
    }

    private static void EnforceRetentionLimit()
    {
        while (s_EventSequences.Count() > SentinelVersion.EVENT_RETENTION_LIMIT)
        {
            s_EventSequences.Remove(0);
            s_EventTimestamps.Remove(0);
            s_EventTypes.Remove(0);
            s_EventPlayerNames.Remove(0);
            s_EventPlayerIds.Remove(0);
            s_EventKillerNames.Remove(0);
            s_EventKillerIds.Remove(0);
            s_EventWeaponClasses.Remove(0);
            s_EventWeaponDisplayNames.Remove(0);
            s_EventCauses.Remove(0);
            s_EventDistances.Remove(0);
            s_EventKillerPositions.Remove(0);
            s_EventVictimPositions.Remove(0);
        }
    }

    private static string BuildUtcTimestamp()
    {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;

        GetYearMonthDayUTC(year, month, day);
        GetHourMinuteSecondUTC(hour, minute, second);

        return year.ToString() + "-" + Pad2(month) + "-" + Pad2(day) + "T" + Pad2(hour) + ":" + Pad2(minute) + ":" + Pad2(second) + "Z";
    }

    private static string Pad2(int value)
    {
        if (value < 10)
        {
            return "0" + value.ToString();
        }

        return value.ToString();
    }

    private static string BuildEventId(int sequence)
    {
        return s_SessionId + "-" + sequence.ToString();
    }

    private static void WritePosition(FileHandle file, string fieldName, vector position, bool trailingComma)
    {
        string suffix = "";
        if (trailingComma)
        {
            suffix = ",";
        }

        FPrintln(file, "      \"" + fieldName + "\": {");
        FPrintln(file, "        \"x\": " + position[0].ToString() + ",");
        FPrintln(file, "        \"y\": " + position[1].ToString() + ",");
        FPrintln(file, "        \"z\": " + position[2].ToString());
        FPrintln(file, "      }" + suffix);
    }

    private static void WriteEventsFile()
    {
        FileHandle file = OpenFile(SentinelConstants.EVENTS_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print(SentinelConstants.LOG_PREFIX + " events.json open failed");
            return;
        }

        int eventCount = s_EventPlayerIds.Count();

        FPrintln(file, "{");
        FPrintln(file, "  \"schema_version\": 2,");
        FPrintln(file, "  \"session_id\": \"" + s_SessionId + "\",");
        FPrintln(file, "  \"retention_limit\": " + SentinelVersion.EVENT_RETENTION_LIMIT.ToString() + ",");
        FPrintln(file, "  \"count\": " + eventCount.ToString() + ",");
        FPrintln(file, "  \"events\": [");

        for (int i = 0; i < eventCount; i++)
        {
            if (i > 0)
            {
                FPrintln(file, "    ,");
            }

            FPrintln(file, "    {");
            FPrintln(file, "      \"event_id\": \"" + BuildEventId(s_EventSequences.Get(i)) + "\",");
            FPrintln(file, "      \"sequence\": " + s_EventSequences.Get(i).ToString() + ",");
            FPrintln(file, "      \"timestamp\": \"" + s_EventTimestamps.Get(i) + "\",");
            FPrintln(file, "      \"type\": \"" + s_EventTypes.Get(i) + "\",");

            if (s_EventTypes.Get(i) == "player_killed")
            {
                FPrintln(file, "      \"cause\": \"" + s_EventCauses.Get(i) + "\",");
                FPrintln(file, "      \"weapon\": {");
                FPrintln(file, "        \"class\": \"" + s_EventWeaponClasses.Get(i) + "\",");
                FPrintln(file, "        \"display_name\": \"" + s_EventWeaponDisplayNames.Get(i) + "\"");
                FPrintln(file, "      },");
                FPrintln(file, "      \"distance_m\": " + s_EventDistances.Get(i).ToString() + ",");
                FPrintln(file, "      \"killer\": {");
                FPrintln(file, "        \"name\": \"" + s_EventKillerNames.Get(i) + "\",");
                FPrintln(file, "        \"id\": \"" + s_EventKillerIds.Get(i) + "\"");
                FPrintln(file, "      },");
                FPrintln(file, "      \"victim\": {");
                FPrintln(file, "        \"name\": \"" + s_EventPlayerNames.Get(i) + "\",");
                FPrintln(file, "        \"id\": \"" + s_EventPlayerIds.Get(i) + "\"");
                FPrintln(file, "      },");
                WritePosition(file, "killer_position", s_EventKillerPositions.Get(i), true);
                WritePosition(file, "victim_position", s_EventVictimPositions.Get(i), false);
            }
            else if (s_EventTypes.Get(i) == "player_died")
            {
                FPrintln(file, "      \"cause\": \"" + s_EventCauses.Get(i) + "\",");
                FPrintln(file, "      \"player\": {");
                FPrintln(file, "        \"name\": \"" + s_EventPlayerNames.Get(i) + "\",");
                FPrintln(file, "        \"id\": \"" + s_EventPlayerIds.Get(i) + "\"");
                FPrintln(file, "      },");
                WritePosition(file, "victim_position", s_EventVictimPositions.Get(i), false);
            }
            else
            {
                FPrintln(file, "      \"player\": {");
                FPrintln(file, "        \"name\": \"" + s_EventPlayerNames.Get(i) + "\",");
                FPrintln(file, "        \"id\": \"" + s_EventPlayerIds.Get(i) + "\"");
                FPrintln(file, "      }");
            }

            FPrintln(file, "    }");
        }

        FPrintln(file, "  ]");
        FPrintln(file, "}");

        CloseFile(file);
    }
}
