#ifdef Sentinel
modded class SentinelPvPZoneManager
{
    protected SentinelZoneDefinition SentinelUIFindZone(string zoneId)
    {
        if (zoneId == "" || !m_Zones)
        {
            return null;
        }

        foreach (SentinelZoneDefinition zone : m_Zones)
        {
            if (zone && zone.id == zoneId)
            {
                return zone;
            }
        }

        return null;
    }

    protected void SentinelUISendZoneState(PlayerBase player, int kind, SentinelZoneDefinition zone)
    {
        if (!player || !player.GetIdentity())
        {
            return;
        }

        string zoneName = "";
        string zoneType = "safe";

        if (zone)
        {
            zoneName = zone.name;
            zoneType = zone.type;
        }

        Param3<int, string, string> payload = new Param3<int, string, string>(kind, zoneName, zoneType);
        GetGame().RPCSingleParam(player, SentinelUIZoneProtocol.RPC_ZONE_WARNING, payload, true, player.GetIdentity());

        Print("[SentinelUI] zone bridge sent kind=" + kind.ToString() + " zone=" + zoneName + " player=" + player.GetIdentity().GetName());
    }

    override protected void LogZoneTransition(PlayerBase player, string previousZoneId, SentinelZoneDefinition currentZone)
    {
        SentinelZoneDefinition previousZone = SentinelUIFindZone(previousZoneId);
        super.LogZoneTransition(player, previousZoneId, currentZone);

        if (currentZone)
        {
            int kind = currentZone.type == "pvp" ? SentinelUIZoneProtocol.KIND_ENTER_PVP : SentinelUIZoneProtocol.KIND_LEAVE_PVP;
            SentinelUISendZoneState(player, kind, currentZone);
            return;
        }

        SentinelUISendZoneState(player, SentinelUIZoneProtocol.KIND_LEAVE_PVP, null);
    }

    protected bool SentinelUIIsMapZone(SentinelZoneDefinition zone)
    {
        if (!zone || !zone.enabled)
        {
            return false;
        }

        return zone.shape == "circle" || zone.shape == "polygon";
    }

    void SentinelUISendMapZones(PlayerBase player)
    {
        if (!player || !player.GetIdentity())
        {
            return;
        }

        int zoneCount = 0;

        if (m_Zones)
        {
            foreach (SentinelZoneDefinition countZone : m_Zones)
            {
                if (SentinelUIIsMapZone(countZone))
                {
                    zoneCount++;
                }
            }
        }

        int revision = GetLoadedRevision();
        ScriptRPC headerRpc = new ScriptRPC();
        headerRpc.Write(SentinelUIZoneProtocol.MAP_PROTOCOL_VERSION);
        headerRpc.Write(revision);
        headerRpc.Write(zoneCount);
        headerRpc.Send(player, SentinelUIZoneProtocol.RPC_MAP_ZONE_HEADER, true, player.GetIdentity());

        if (!m_Zones)
        {
            return;
        }

        foreach (SentinelZoneDefinition zone : m_Zones)
        {
            if (!SentinelUIIsMapZone(zone))
            {
                continue;
            }

            ref array<float> coordinates = new array<float>();

            if (zone.shape == "circle")
            {
                coordinates.Insert(zone.centre[0]);
                coordinates.Insert(zone.centre[1]);
            }
            else
            {
                foreach (array<float> point : zone.points)
                {
                    coordinates.Insert(point[0]);
                    coordinates.Insert(point[1]);
                }
            }

            ScriptRPC zoneRpc = new ScriptRPC();
            zoneRpc.Write(SentinelUIZoneProtocol.MAP_PROTOCOL_VERSION);
            zoneRpc.Write(revision);
            zoneRpc.Write(zone.id);
            zoneRpc.Write(zone.name);
            zoneRpc.Write(zone.type);
            zoneRpc.Write(zone.shape);
            zoneRpc.Write(zone.priority);
            zoneRpc.Write(zone.radius);
            zoneRpc.Write(coordinates);
            zoneRpc.Send(player, SentinelUIZoneProtocol.RPC_MAP_ZONE_DATA, true, player.GetIdentity());
        }

        Print("[SentinelUI] sent map zones revision=" + revision.ToString() + " count=" + zoneCount.ToString() + " player=" + player.GetIdentity().GetName());
    }
}
#endif
