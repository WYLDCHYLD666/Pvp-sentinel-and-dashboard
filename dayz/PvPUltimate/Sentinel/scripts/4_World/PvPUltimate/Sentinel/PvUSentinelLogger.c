// PvUSentinelLogger.c
// Sentinel telemetry module for PvPUltimate.
// v0.1.0 goal: export dashboard JSON files without changing PvP enforcement behaviour.

class PvUSentinelStatus
{
    string server_name;
    string mode;
    int online_players;

    void PvUSentinelStatus()
    {
        server_name = "DayZ PvP Sentinel";
        mode = "UNKNOWN";
        online_players = 0;
    }
}

class PvUSentinelIncident
{
    string type;
    string attacker;
    string victim;
    string reason;
    bool legal;

    void PvUSentinelIncident()
    {
        type = "incident";
        attacker = "";
        victim = "";
        reason = "";
        legal = false;
    }
}

class PvUSentinelPunishment
{
    string type;
    string player;
    string action;
    string reason;

    void PvUSentinelPunishment()
    {
        type = "punishment";
        player = "";
        action = "";
        reason = "";
    }
}

class PvUSentinelLogger
{
    static const string SENTINEL_DIR = "$profile:PvPSentinel/";
    static const string STATUS_FILE = "$profile:PvPSentinel/status.json";
    static const string PLAYERS_FILE = "$profile:PvPSentinel/players.json";
    static const string INCIDENTS_FILE = "$profile:PvPSentinel/incidents.json";
    static const string PUNISHMENTS_FILE = "$profile:PvPSentinel/punishments.json";

    static void Init()
    {
        EnsureDirectory();
        WriteStatus("UNKNOWN", 0);
        WriteEmptyArrayFile(PLAYERS_FILE);
        WriteEmptyArrayFile(INCIDENTS_FILE);
        WriteEmptyArrayFile(PUNISHMENTS_FILE);
    }

    static void WriteStatus(string mode, int onlinePlayers)
    {
        EnsureDirectory();

        PvUSentinelStatus status = new PvUSentinelStatus();
        status.mode = mode;
        status.online_players = onlinePlayers;

        JsonFileLoader<PvUSentinelStatus>.JsonSaveFile(STATUS_FILE, status);
    }

    static void LogIncident(string attackerId, string victimId, string reason, bool legal)
    {
        EnsureDirectory();

        PvUSentinelIncident incident = new PvUSentinelIncident();
        incident.attacker = attackerId;
        incident.victim = victimId;
        incident.reason = reason;
        incident.legal = legal;

        JsonFileLoader<PvUSentinelIncident>.JsonSaveFile(INCIDENTS_FILE, incident);
    }

    static void LogPunishment(string playerId, string action, string reason)
    {
        EnsureDirectory();

        PvUSentinelPunishment punishment = new PvUSentinelPunishment();
        punishment.player = playerId;
        punishment.action = action;
        punishment.reason = reason;

        JsonFileLoader<PvUSentinelPunishment>.JsonSaveFile(PUNISHMENTS_FILE, punishment);
    }

    protected static void EnsureDirectory()
    {
        if (!FileExist(SENTINEL_DIR))
        {
            MakeDirectory(SENTINEL_DIR);
        }
    }

    protected static void WriteEmptyArrayFile(string path)
    {
        FileHandle file = OpenFile(path, FileMode.WRITE);
        if (file != 0)
        {
            FPrint(file, "[]");
            CloseFile(file);
        }
    }
}
