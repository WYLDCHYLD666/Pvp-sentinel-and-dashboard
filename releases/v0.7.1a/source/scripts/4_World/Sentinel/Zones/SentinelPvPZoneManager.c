class SentinelPvPZoneManager
{
    protected static ref SentinelPvPZoneManager s_Instance;
    protected bool m_Initialized;

    static SentinelPvPZoneManager GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SentinelPvPZoneManager();
        }

        return s_Instance;
    }

    void Init()
    {
        if (m_Initialized)
        {
            return;
        }

        m_Initialized = true;
        SentinelFileUtils.EnsureProfileDirectory();
        EnsureDefaultZonesFile();
        SentinelStatusExporter.Write("SAFE", GetOnlinePlayerCount(), false);
        Print(SentinelConstants.LOG_PREFIX + " zones initialized");
    }

    protected void EnsureDefaultZonesFile()
    {
        if (FileExist(SentinelConstants.ZONES_FILE))
        {
            return;
        }

        FileHandle file = OpenFile(SentinelConstants.ZONES_FILE, FileMode.WRITE);
        if (file != 0)
        {
            FPrintln(file, "[");
            FPrintln(file, "  {");
            FPrintln(file, "    \"name\": \"Example PvP Zone\",");
            FPrintln(file, "    \"x\": 7500,");
            FPrintln(file, "    \"z\": 7500,");
            FPrintln(file, "    \"radius\": 500,");
            FPrintln(file, "    \"enabled\": false");
            FPrintln(file, "  }");
            FPrintln(file, "]");
            CloseFile(file);
        }
    }

    protected int GetOnlinePlayerCount()
    {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        return players.Count();
    }
}
