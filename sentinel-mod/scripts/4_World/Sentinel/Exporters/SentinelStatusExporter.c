class SentinelStatusExporter
{
    static void Write(string mode, int onlinePlayers, bool weekendPvP)
    {
        FileHandle file = OpenFile(SentinelConstants.STATUS_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print(SentinelConstants.LOG_PREFIX + " status.json open failed");
            return;
        }

        int serverUptime = GetGame().GetTime() / 1000;

        FPrintln(file, "{");
        FPrintln(file, "  \"version\": \"" + SentinelVersion.VERSION + "\",");
        FPrintln(file, "  \"build_id\": \"" + SentinelVersion.BUILD_ID + "\",");
        FPrintln(file, "  \"mode\": \"" + mode + "\",");
        FPrintln(file, "  \"online_players\": " + onlinePlayers.ToString() + ",");
        FPrintln(file, "  \"weekend_pvp\": " + SentinelFileUtils.BoolToJson(weekendPvP) + ",");
        FPrintln(file, "  \"heartbeat\": " + serverUptime.ToString());
        FPrintln(file, "}");

        CloseFile(file);
    }
}
