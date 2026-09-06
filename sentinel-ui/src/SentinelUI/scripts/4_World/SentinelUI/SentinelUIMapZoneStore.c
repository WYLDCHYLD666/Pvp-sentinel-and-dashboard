class SentinelUIMapZoneData
{
    string Id;
    string Name;
    string Type;
    string GeometryKind;
    int Priority;
    float Radius;
    ref array<float> Coordinates;

    void SentinelUIMapZoneData()
    {
        Coordinates = new array<float>();
    }
}

class SentinelUIMapZoneStore
{
    static const int MAX_ZONES = 256;
    static const int MAX_COORDINATE_VALUES = 2048;

    protected static ref array<ref SentinelUIMapZoneData> s_Zones = new array<ref SentinelUIMapZoneData>();
    protected static ref array<ref SentinelUIMapZoneData> s_PendingZones;
    protected static int s_PendingRevision = -1;
    protected static int s_ExpectedCount;
    protected static int s_ReceivedCount;
    protected static int s_Revision = -1;
    protected static int s_Sequence;

    static void RequestFromServer()
    {
        if (!GetGame() || !GetGame().IsClient())
        {
            return;
        }

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        if (!player || !player.GetIdentity())
        {
            return;
        }

        ScriptRPC requestRpc = new ScriptRPC();
        requestRpc.Write(SentinelUIZoneProtocol.MAP_PROTOCOL_VERSION);
        requestRpc.Send(player, SentinelUIZoneProtocol.RPC_MAP_ZONE_REQUEST, true, null);
        Print("[SentinelUI] requested map zones");
    }

    static void BeginResponse(int revision, int zoneCount)
    {
        if (zoneCount < 0 || zoneCount > MAX_ZONES)
        {
            Print("[SentinelUI] rejected invalid map-zone count " + zoneCount.ToString());
            s_PendingZones = null;
            return;
        }

        s_PendingRevision = revision;
        s_ExpectedCount = zoneCount;
        s_ReceivedCount = 0;
        s_PendingZones = new array<ref SentinelUIMapZoneData>();

        if (zoneCount == 0)
        {
            CommitResponse();
        }
    }

    static void ReceiveZone(int revision, string zoneId, string zoneName, string zoneType, string zoneShape, int zonePriority, float zoneRadius, array<float> coordinates)
    {
        if (!s_PendingZones || revision != s_PendingRevision || s_ReceivedCount >= s_ExpectedCount)
        {
            return;
        }

        s_ReceivedCount++;

        if (IsValidGeometry(zoneShape, zoneRadius, coordinates))
        {
            ref SentinelUIMapZoneData zone = new SentinelUIMapZoneData();
            zone.Id = zoneId;
            zone.Name = zoneName;
            zone.Type = zoneType;
            zone.GeometryKind = zoneShape;
            zone.Priority = zonePriority;
            zone.Radius = zoneRadius;
            zone.Coordinates.Copy(coordinates);
            s_PendingZones.Insert(zone);
        }
        else
        {
            Print("[SentinelUI] skipped invalid map geometry for zone " + zoneId);
        }

        if (s_ReceivedCount >= s_ExpectedCount)
        {
            CommitResponse();
        }
    }

    protected static bool IsValidGeometry(string geometryKind, float radius, array<float> coordinates)
    {
        if (!coordinates || coordinates.Count() > MAX_COORDINATE_VALUES)
        {
            return false;
        }

        if (geometryKind == "circle")
        {
            return coordinates.Count() == 2 && radius > 0;
        }

        if (geometryKind == "polygon")
        {
            return coordinates.Count() >= 6 && (coordinates.Count() % 2) == 0;
        }

        return false;
    }

    protected static void CommitResponse()
    {
        s_Zones = s_PendingZones;
        s_PendingZones = null;
        s_Revision = s_PendingRevision;
        s_Sequence++;
        Print("[SentinelUI] map zones ready revision=" + s_Revision.ToString() + " count=" + s_Zones.Count().ToString());
    }

    static array<ref SentinelUIMapZoneData> GetZones()
    {
        return s_Zones;
    }

    static int GetRevision()
    {
        return s_Revision;
    }

    static int GetSequence()
    {
        return s_Sequence;
    }

    static void Reset()
    {
        s_Zones = new array<ref SentinelUIMapZoneData>();
        s_PendingZones = null;
        s_PendingRevision = -1;
        s_ExpectedCount = 0;
        s_ReceivedCount = 0;
        s_Revision = -1;
        s_Sequence++;
    }
}
