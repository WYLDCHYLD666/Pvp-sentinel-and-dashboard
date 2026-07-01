// PvUSentinelLogger.c
// Sentinel telemetry module for PvPUltimate.
// v0.1.0 goal: export JSON-style dashboard files without changing PvP enforcement behaviour.

class PvUSentinelLogger
{
    static const string SENTINEL_DIR = "$profile:PvPSentinel/";
    static const string STATUS_FILE = "$profile:PvPSentinel/status.json";
    static const string PLAYERS_FILE = "$profile:PvPSentinel/players.json";
    static const string INCIDENTS_FILE = "$profile:PvPSentinel/incidents.json";
    static const string PUNISHMENTS_FILE = "$profile:PvPSentinel/punishments.json";

    static void Init()
    {
        if (!FileExist(SENTINEL_DIR))
        {
            MakeDirectory(SENTINEL_DIR);
        }

        WriteStatus("UNKNOWN", 0);
        WriteTextFile(PLAYERS_FILE, "[]");
        WriteTextFile(INCIDENTS_FILE, "[]");
        WriteTextFile(PUNISHMENTS_FILE, "[]");
    }

    static void WriteStatus(string mode, int onlinePlayers)
    {
        string json = "{\n";
        json += "    \"server_name\": \"DayZ PvP Sentinel\",\n";
        json += "    \"mode\": \"" + EscapeJson(mode) + "\",\n";
        json += "    \"online_players\": " + onlinePlayers.ToString() + "\n";
        json += "}\n";

        WriteTextFile(STATUS_FILE, json);
    }

    static void LogIncident(string attackerId, string victimId, string reason, bool legal)
    {
        string json = "{\n";
        json += "    \"type\": \"incident\",\n";
        json += "    \"attacker\": \"" + EscapeJson(attackerId) + "\",\n";
        json += "    \"victim\": \"" + EscapeJson(victimId) + "\",\n";
        json += "    \"reason\": \"" + EscapeJson(reason) + "\",\n";
        json += "    \"legal\": " + BoolToJson(legal) + "\n";
        json += "}\n";

        WriteTextFile(INCIDENTS_FILE, json);
    }

    static void LogPunishment(string playerId, string action, string reason)
    {
        string json = "{\n";
        json += "    \"type\": \"punishment\",\n";
        json += "    \"player\": \"" + EscapeJson(playerId) + "\",\n";
        json += "    \"action\": \"" + EscapeJson(action) + "\",\n";
        json += "    \"reason\": \"" + EscapeJson(reason) + "\"\n";
        json += "}\n";

        WriteTextFile(PUNISHMENTS_FILE, json);
    }

    protected static void WriteTextFile(string path, string content)
    {
        FileHandle file = OpenFile(path, FileMode.WRITE);
        if (file != 0)
        {
            FPrint(file, content);
            CloseFile(file);
        }
    }

    protected static string EscapeJson(string value)
    {
        value.Replace("\\", "\\\\");
        value.Replace("\"", "\\\"");
        return value;
    }

    protected static string BoolToJson(bool value)
    {
        if (value)
        {
            return "true";
        }

        return "false";
    }
}
