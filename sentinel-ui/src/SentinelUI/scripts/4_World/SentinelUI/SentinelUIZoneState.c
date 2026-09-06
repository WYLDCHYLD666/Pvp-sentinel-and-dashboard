class SentinelUIZoneProtocol
{
    static const int RPC_ZONE_WARNING = 781943;
    static const int RPC_MAP_ZONE_REQUEST = 781944;
    static const int RPC_MAP_ZONE_HEADER = 781945;
    static const int RPC_MAP_ZONE_DATA = 781946;
    static const int RPC_RULE_REQUEST = 781947;
    static const int RPC_PURGE_STATE = 781948;
    static const int MAP_PROTOCOL_VERSION = 1;
    static const int RULE_PROTOCOL_VERSION = 1;
    static const int KIND_ENTER_PVP = 1;
    static const int KIND_LEAVE_PVP = 2;
    static const int KIND_PURGE_COUNTDOWN = 20;
    static const int KIND_PURGE_START = 21;
    static const int KIND_PURGE_END = 22;
    static const int KIND_PURGE_SYNC = 23;
}

class SentinelUIZoneState
{
    protected static int s_Sequence;
    protected static bool s_IsPvP;
    protected static string s_ZoneName;
    protected static string s_ZoneType = "safe";

    static void Apply(int kind, string zoneName, string zoneType)
    {
        zoneType.ToLower();

        if (zoneType == "")
        {
            if (kind == SentinelUIZoneProtocol.KIND_ENTER_PVP)
            {
                zoneType = "pvp";
            }
            else
            {
                zoneType = "safe";
            }
        }

        if (kind == SentinelUIZoneProtocol.KIND_ENTER_PVP)
        {
            s_IsPvP = true;
            s_ZoneName = zoneName;
        }
        else if (kind == SentinelUIZoneProtocol.KIND_LEAVE_PVP)
        {
            s_IsPvP = false;
            s_ZoneName = zoneName;
        }
        else
        {
            return;
        }

        s_ZoneType = zoneType;
        s_Sequence++;
    }

    static int GetSequence()
    {
        return s_Sequence;
    }

    static bool IsPvP()
    {
        return s_IsPvP;
    }

    static string GetZoneName()
    {
        return s_ZoneName;
    }

    static string GetZoneType()
    {
        return s_ZoneType;
    }
}

class SentinelUIPurgeState
{
    protected static int s_Sequence;
    protected static int s_SirenSequence;
    protected static int s_LastKind;
    protected static int s_CountdownSeconds = -1;
    protected static bool s_Active;
    protected static string s_EventName = "Purge";

    static void Apply(int kind, int value, string eventName)
    {
        if (eventName != "")
        {
            s_EventName = eventName;
        }

        if (kind == SentinelUIZoneProtocol.KIND_PURGE_COUNTDOWN)
        {
            s_CountdownSeconds = value;
            s_Active = false;
        }
        else if (kind == SentinelUIZoneProtocol.KIND_PURGE_START)
        {
            s_CountdownSeconds = -1;
            s_Active = true;
            s_SirenSequence++;
        }
        else if (kind == SentinelUIZoneProtocol.KIND_PURGE_END)
        {
            s_CountdownSeconds = -1;
            s_Active = false;
        }
        else if (kind == SentinelUIZoneProtocol.KIND_PURGE_SYNC)
        {
            s_CountdownSeconds = -1;
            s_Active = value == 1;
        }
        else
        {
            return;
        }

        s_LastKind = kind;
        s_Sequence++;
    }

    static void RequestFromServer()
    {
        if (!GetGame() || !GetGame().IsClient())
        {
            return;
        }

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        if (!player)
        {
            return;
        }

        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(SentinelUIZoneProtocol.RULE_PROTOCOL_VERSION);
        rpc.Send(player, SentinelUIZoneProtocol.RPC_RULE_REQUEST, true);
    }

    static void Reset()
    {
        s_Sequence = 0;
        s_SirenSequence = 0;
        s_LastKind = 0;
        s_CountdownSeconds = -1;
        s_Active = false;
        s_EventName = "Purge";
    }

    static int GetSequence()
    {
        return s_Sequence;
    }

    static int GetSirenSequence()
    {
        return s_SirenSequence;
    }

    static int GetLastKind()
    {
        return s_LastKind;
    }

    static int GetCountdownSeconds()
    {
        return s_CountdownSeconds;
    }

    static bool IsActive()
    {
        return s_Active;
    }

    static string GetEventName()
    {
        return s_EventName;
    }
}

modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if (rpc_type == SentinelUIZoneProtocol.RPC_RULE_REQUEST && GetGame() && GetGame().IsServer())
        {
            int requestedRuleProtocol;

            if (!ctx.Read(requestedRuleProtocol))
            {
                Print("[SentinelUI] unable to read rule-state request");
                return;
            }

            if (requestedRuleProtocol != SentinelUIZoneProtocol.RULE_PROTOCOL_VERSION)
            {
                Print("[SentinelUI] rejected rule-state request with unsupported protocol " + requestedRuleProtocol.ToString());
                return;
            }

            SentinelUIScheduleBridge.GetInstance().SendStateToPlayer(this);
            return;
        }

#ifdef Sentinel
        if (rpc_type == SentinelUIZoneProtocol.RPC_MAP_ZONE_REQUEST && GetGame() && GetGame().IsServer())
        {
            int requestedProtocol;

            if (!ctx.Read(requestedProtocol))
            {
                Print("[SentinelUI] unable to read map-zone request");
                return;
            }

            if (requestedProtocol != SentinelUIZoneProtocol.MAP_PROTOCOL_VERSION)
            {
                Print("[SentinelUI] rejected map-zone request with unsupported protocol " + requestedProtocol.ToString());
                return;
            }

            SentinelPvPZoneManager.GetInstance().SentinelUISendMapZones(this);
            return;
        }
#endif

        if (rpc_type == SentinelUIZoneProtocol.RPC_ZONE_WARNING)
        {
            if (!GetGame() || !GetGame().IsClient())
            {
                return;
            }

            Param3<int, string, string> payload;

            if (!ctx.Read(payload))
            {
                Print("[SentinelUI] unable to read zone status RPC");
                return;
            }

            SentinelUIZoneState.Apply(payload.param1, payload.param2, payload.param3);
            Print("[SentinelUI] zone status RPC received kind=" + payload.param1.ToString() + " zone=" + payload.param2 + " type=" + payload.param3);
            return;
        }

        if (rpc_type == SentinelUIZoneProtocol.RPC_PURGE_STATE)
        {
            if (!GetGame() || !GetGame().IsClient())
            {
                return;
            }

            Param3<int, int, string> purgePayload;

            if (!ctx.Read(purgePayload))
            {
                Print("[SentinelUI] unable to read Purge state RPC");
                return;
            }

            SentinelUIPurgeState.Apply(purgePayload.param1, purgePayload.param2, purgePayload.param3);
            Print("[SentinelUI] Purge RPC received kind=" + purgePayload.param1.ToString() + " value=" + purgePayload.param2.ToString() + " event=" + purgePayload.param3);
            return;
        }

        if (!GetGame() || !GetGame().IsClient())
        {
            return;
        }

        if (rpc_type == SentinelUIZoneProtocol.RPC_MAP_ZONE_HEADER)
        {
            int headerProtocol;
            int revision;
            int zoneCount;

            if (!ctx.Read(headerProtocol) || !ctx.Read(revision) || !ctx.Read(zoneCount))
            {
                Print("[SentinelUI] unable to read map-zone header");
                return;
            }

            if (headerProtocol != SentinelUIZoneProtocol.MAP_PROTOCOL_VERSION)
            {
                Print("[SentinelUI] ignored map-zone header with unsupported protocol " + headerProtocol.ToString());
                return;
            }

            SentinelUIMapZoneStore.BeginResponse(revision, zoneCount);
            return;
        }

        if (rpc_type == SentinelUIZoneProtocol.RPC_MAP_ZONE_DATA)
        {
            int dataProtocol;
            int dataRevision;
            string zoneId;
            string zoneName;
            string zoneType;
            string zoneShape;
            int zonePriority;
            float zoneRadius;
            ref array<float> coordinates = new array<float>();

            if (!ctx.Read(dataProtocol) || !ctx.Read(dataRevision) || !ctx.Read(zoneId) || !ctx.Read(zoneName) || !ctx.Read(zoneType) || !ctx.Read(zoneShape) || !ctx.Read(zonePriority) || !ctx.Read(zoneRadius) || !ctx.Read(coordinates))
            {
                Print("[SentinelUI] unable to read map-zone data");
                return;
            }

            if (dataProtocol != SentinelUIZoneProtocol.MAP_PROTOCOL_VERSION)
            {
                Print("[SentinelUI] ignored map-zone data with unsupported protocol " + dataProtocol.ToString());
                return;
            }

            SentinelUIMapZoneStore.ReceiveZone(dataRevision, zoneId, zoneName, zoneType, zoneShape, zonePriority, zoneRadius, coordinates);
        }
    }
}
