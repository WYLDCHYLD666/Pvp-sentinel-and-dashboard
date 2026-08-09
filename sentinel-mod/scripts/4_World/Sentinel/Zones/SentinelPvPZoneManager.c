class SentinelPvPZoneManager
{
    protected static ref SentinelPvPZoneManager s_Instance;

    protected bool m_Initialized;
    protected bool m_ContractLoaded;
    protected int m_LoadedRevision;
    protected string m_LoadedHash;
    protected ref array<ref SentinelZoneDefinition> m_Zones;
    protected ref map<string, string> m_PlayerZoneState;

    static SentinelPvPZoneManager GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SentinelPvPZoneManager();
        }

        return s_Instance;
    }

    void SentinelPvPZoneManager()
    {
        m_Zones = new array<ref SentinelZoneDefinition>();
        m_PlayerZoneState = new map<string, string>();
        m_LoadedRevision = 0;
        m_LoadedHash = "";
    }

    void Init()
    {
        if (m_Initialized)
        {
            return;
        }

        m_Initialized = true;

        if (!SentinelFileUtils.EnsureProfileDirectory())
        {
            Print(SentinelConstants.LOG_PREFIX + " ERROR zone adapter could not access profile directory");
            return;
        }

        EnsureDefaultZonesFile();
        ReloadZones(true);

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SentinelPvPZoneManager.ReloadTick, SentinelConstants.ZONE_RELOAD_INTERVAL_MS, true);

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SentinelPvPZoneManager.PlayerZoneTick, SentinelConstants.ZONE_PLAYER_SCAN_INTERVAL_MS, true);

        Print(SentinelConstants.LOG_PREFIX + " zone contract adapter initialized");
    }

    static void ReloadTick()
    {
        SentinelPvPZoneManager.GetInstance().ReloadZones(false);
    }

    static void PlayerZoneTick()
    {
        SentinelPvPZoneManager.GetInstance().ScanPlayerZones();
    }

    void ReloadZones(bool force)
    {
        if (!FileExist(SentinelConstants.ZONES_FILE))
        {
            if (force)
            {
                Print(SentinelConstants.LOG_PREFIX + " zones file missing: " + SentinelConstants.ZONES_FILE);
            }

            return;
        }

        ref SentinelZoneContract contractData;
        contractData = LoadContract();

        if (contractData)
        {
            if (!force && contractData.revision == m_LoadedRevision && contractData.content_hash == m_LoadedHash)
            {
                return;
            }

            if (!ValidateContract(contractData))
            {
                Print(SentinelConstants.LOG_PREFIX + " ERROR rejected invalid zone contract revision " + contractData.revision.ToString());
                return;
            }

            ApplyContract(contractData);
            return;
        }

        if (force)
        {
            LoadLegacyZones();
        }
    }

    protected SentinelZoneContract LoadContract()
    {
        ref SentinelZoneContract contractData;
        contractData = new SentinelZoneContract();

        JsonFileLoader<SentinelZoneContract>.JsonLoadFile(SentinelConstants.ZONES_FILE, contractData);

        if (!contractData)
        {
            return null;
        }

        if (contractData.schema != 1)
        {
            return null;
        }

        if (contractData.contract != SentinelConstants.ZONE_CONTRACT_NAME)
        {
            return null;
        }

        return contractData;
    }

    protected bool ValidateContract(SentinelZoneContract data)
    {
        if (!data)
        {
            return false;
        }

        if (data.revision < 1)
        {
            Print(SentinelConstants.LOG_PREFIX + " ERROR zone revision must be greater than zero");
            return false;
        }

        if (!data.zones)
        {
            Print(SentinelConstants.LOG_PREFIX + " ERROR zone collection is null");
            return false;
        }

        ref map<string, bool> seenIds = new map<string, bool>();

        foreach (SentinelZoneDefinition zone : data.zones)
        {
            if (!ValidateZone(zone, seenIds))
            {
                return false;
            }
        }

        return true;
    }

    protected bool ValidateZone(SentinelZoneDefinition zone, map<string, bool> seenIds)
    {
        if (!zone)
        {
            Print(SentinelConstants.LOG_PREFIX + " ERROR null zone in contract");
            return false;
        }

        if (zone.id == "")
        {
            Print(SentinelConstants.LOG_PREFIX + " ERROR zone has no stable ID");
            return false;
        }

        if (seenIds.Contains(zone.id))
        {
            Print(SentinelConstants.LOG_PREFIX + " ERROR duplicate zone ID: " + zone.id);
            return false;
        }

        seenIds.Insert(zone.id, true);

        if (zone.shape == "circle")
        {
            if (!zone.centre || zone.centre.Count() < 2 || zone.radius <= 0)
            {
                Print(SentinelConstants.LOG_PREFIX + " ERROR invalid circle geometry: " + zone.id);
                return false;
            }

            return true;
        }

        if (zone.shape == "polygon")
        {
            if (!zone.points || zone.points.Count() < 3)
            {
                Print(SentinelConstants.LOG_PREFIX + " ERROR polygon needs at least three points: " + zone.id);
                return false;
            }

            foreach (array<float> point : zone.points)
            {
                if (!point || point.Count() < 2)
                {
                    Print(SentinelConstants.LOG_PREFIX + " ERROR invalid polygon point: " + zone.id);
                    return false;
                }
            }

            return true;
        }

        Print(SentinelConstants.LOG_PREFIX + " ERROR unsupported zone shape: " + zone.shape);
        return false;
    }

    protected void ApplyContract(SentinelZoneContract data)
    {
        ref array<ref SentinelZoneDefinition> nextZones = new array<ref SentinelZoneDefinition>();

        foreach (SentinelZoneDefinition zone : data.zones)
        {
            nextZones.Insert(zone);
        }

        m_Zones = nextZones;
        m_LoadedRevision = data.revision;
        m_LoadedHash = data.content_hash;
        m_ContractLoaded = true;

        int enabledCount = GetEnabledZoneCount();

        Print(SentinelConstants.LOG_PREFIX + " loaded zone contract revision " + m_LoadedRevision.ToString() + " with " + m_Zones.Count().ToString() + " zones (" + enabledCount.ToString() + " enabled)");

        SentinelZoneStatusExporter.Write(m_LoadedRevision, m_LoadedHash, m_Zones.Count(), enabledCount, "loaded");
    }

    protected void LoadLegacyZones()
    {
        ref array<ref SentinelLegacyZoneDefinition> legacyZones = new array<ref SentinelLegacyZoneDefinition>();

        JsonFileLoader<array<ref SentinelLegacyZoneDefinition>>.JsonLoadFile(SentinelConstants.ZONES_FILE, legacyZones);

        if (!legacyZones || legacyZones.Count() == 0)
        {
            Print(SentinelConstants.LOG_PREFIX + " no valid contract or legacy zones were loaded");
            return;
        }

        ref array<ref SentinelZoneDefinition> converted = new array<ref SentinelZoneDefinition>();

        int index = 0;

        foreach (SentinelLegacyZoneDefinition legacy : legacyZones)
        {
            if (!legacy)
            {
                continue;
            }

            ref SentinelZoneDefinition zone = new SentinelZoneDefinition();
            zone.id = "legacy-" + index.ToString();
            zone.name = legacy.name;
            zone.type = "pvp";
            zone.shape = "circle";
            zone.enabled = legacy.enabled;
            zone.priority = 100;
            zone.centre.Insert(legacy.x);
            zone.centre.Insert(legacy.z);
            zone.radius = legacy.radius;
            converted.Insert(zone);
            index++;
        }

        m_Zones = converted;
        m_LoadedRevision = 0;
        m_LoadedHash = "legacy";
        m_ContractLoaded = false;

        Print(SentinelConstants.LOG_PREFIX + " loaded " + converted.Count().ToString() + " legacy circle zones");

        SentinelZoneStatusExporter.Write(0, "legacy", converted.Count(), GetEnabledZoneCount(), "legacy");
    }

    bool IsPositionInPvPZone(vector position)
    {
        SentinelZoneDefinition zone = GetWinningZone(position);

        if (!zone)
        {
            return false;
        }

        return zone.type == "pvp";
    }

    SentinelZoneDefinition GetWinningZone(vector position)
    {
        SentinelZoneDefinition winner = null;

        foreach (SentinelZoneDefinition zone : m_Zones)
        {
            if (!zone || !zone.enabled)
            {
                continue;
            }

            if (!ContainsPosition(zone, position))
            {
                continue;
            }

            if (!winner || zone.priority > winner.priority)
            {
                winner = zone;
            }
        }

        return winner;
    }

    bool ContainsPosition(SentinelZoneDefinition zone, vector position)
    {
        if (!zone || !zone.enabled)
        {
            return false;
        }

        if (zone.shape == "circle")
        {
            return ContainsCircle(zone, position);
        }

        if (zone.shape == "polygon")
        {
            return ContainsPolygon(zone, position);
        }

        return false;
    }

    protected bool ContainsCircle(SentinelZoneDefinition zone, vector position)
    {
        if (!zone.centre || zone.centre.Count() < 2)
        {
            return false;
        }

        float dx = position[0] - zone.centre[0];
        float dz = position[2] - zone.centre[1];

        return ((dx * dx) + (dz * dz)) <= (zone.radius * zone.radius);
    }

    protected bool ContainsPolygon(SentinelZoneDefinition zone, vector position)
    {
        if (!zone.points || zone.points.Count() < 3)
        {
            return false;
        }

        float x = position[0];
        float z = position[2];
        bool inside = false;
        int j = zone.points.Count() - 1;

        for (int i = 0; i < zone.points.Count(); i++)
        {
            array<float> pointI = zone.points[i];
            array<float> pointJ = zone.points[j];

            float xi = pointI[0];
            float zi = pointI[1];
            float xj = pointJ[0];
            float zj = pointJ[1];

            bool crosses = ((zi > z) != (zj > z));

            if (crosses)
            {
                float denominator = zj - zi;

                if (Math.AbsFloat(denominator) < 0.0001)
                {
                    denominator = 0.0001;
                }

                float boundaryX = ((xj - xi) * (z - zi) / denominator) + xi;

                if (x < boundaryX)
                {
                    inside = !inside;
                }
            }

            j = i;
        }

        return inside;
    }

    bool ArePositionsInSamePvPZone(vector firstPosition, vector secondPosition)
    {
        SentinelZoneDefinition firstZone;
        SentinelZoneDefinition secondZone;

        firstZone = GetWinningZone(firstPosition);
        secondZone = GetWinningZone(secondPosition);

        if (!firstZone)
        {
            return false;
        }

        if (!secondZone)
        {
            return false;
        }

        if (firstZone.type != "pvp")
        {
            return false;
        }

        if (secondZone.type != "pvp")
        {
            return false;
        }

        return firstZone.id == secondZone.id;
    }

    int GetLoadedRevision()
    {
        return m_LoadedRevision;
    }

    string GetLoadedHash()
    {
        return m_LoadedHash;
    }

    int GetZoneCount()
    {
        if (!m_Zones)
        {
            return 0;
        }

        return m_Zones.Count();
    }

    int GetEnabledZoneCount()
    {
        int count = 0;

        foreach (SentinelZoneDefinition zone : m_Zones)
        {
            if (zone && zone.enabled)
            {
                count++;
            }
        }

        return count;
    }

    protected void ScanPlayerZones()
    {
        ref array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        ref map<string, bool> onlineIds = new map<string, bool>();

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);

            if (!player || !player.GetIdentity())
            {
                continue;
            }

            string playerId = player.GetIdentity().GetId();
            onlineIds.Insert(playerId, true);

            SentinelZoneDefinition winningZone = GetWinningZone(player.GetPosition());
            string currentZoneId = "";

            if (winningZone)
            {
                currentZoneId = winningZone.id;
            }

            string previousZoneId = "";

            if (m_PlayerZoneState.Contains(playerId))
            {
                previousZoneId = m_PlayerZoneState.Get(playerId);
            }

            if (currentZoneId != previousZoneId)
            {
                LogZoneTransition(player, previousZoneId, winningZone);
                m_PlayerZoneState.Set(playerId, currentZoneId);
            }
        }

        ref array<string> staleIds = new array<string>();

        foreach (string trackedId, string trackedZone : m_PlayerZoneState)
        {
            if (!onlineIds.Contains(trackedId))
            {
                staleIds.Insert(trackedId);
            }
        }

        foreach (string staleId : staleIds)
        {
            m_PlayerZoneState.Remove(staleId);
        }
    }

    protected void LogZoneTransition(PlayerBase player, string previousZoneId, SentinelZoneDefinition currentZone)
    {
        string playerName = "unknown";

        if (player.GetIdentity())
        {
            playerName = player.GetIdentity().GetName();
        }

        if (previousZoneId != "")
        {
            Print(SentinelConstants.LOG_PREFIX + " player " + playerName + " left zone " + previousZoneId);
        }

        if (currentZone)
        {
            Print(SentinelConstants.LOG_PREFIX + " player " + playerName + " entered zone " + currentZone.id + " (" + currentZone.name + ")");
        }
    }

    protected void EnsureDefaultZonesFile()
    {
        if (FileExist(SentinelConstants.ZONES_FILE))
        {
            return;
        }

        ref SentinelZoneContract defaultContract = new SentinelZoneContract();
        defaultContract.schema = 1;
        defaultContract.contract = SentinelConstants.ZONE_CONTRACT_NAME;
        defaultContract.revision = 1;
        defaultContract.world = "chernarusplus";
        defaultContract.world_size = 15360;
        defaultContract.generated_at = "";
        defaultContract.content_hash = "default";

        JsonFileLoader<SentinelZoneContract>.JsonSaveFile(SentinelConstants.ZONES_FILE, defaultContract);

        if (!FileExist(SentinelConstants.ZONES_FILE))
        {
            Print(SentinelConstants.LOG_PREFIX + " ERROR unable to create default zones file");
        }
    }
}
