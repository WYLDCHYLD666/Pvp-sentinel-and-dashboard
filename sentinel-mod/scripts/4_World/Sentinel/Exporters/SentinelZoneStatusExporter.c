class SentinelZoneStatusExporter
{
    static void Write(int revision, string contentHash, int zoneCount, int enabledCount, string state)
    {
        FileHandle file = OpenFile(SentinelConstants.ZONE_STATUS_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print(SentinelConstants.LOG_PREFIX + " zone_status.json open failed");
            return;
        }

        int uptime = GetGame().GetTime() / 1000;

        FPrintln(file, "{");
        FPrintln(file, "  \"adapter_version\": \"" + SentinelVersion.VERSION + "\",");
        FPrintln(file, "  \"adapter_build_id\": \"" + SentinelVersion.BUILD_ID + "\",");
        FPrintln(file, "  \"state\": \"" + state + "\",");
        FPrintln(file, "  \"revision\": " + revision.ToString() + ",");
        FPrintln(file, "  \"content_hash\": \"" + contentHash + "\",");
        FPrintln(file, "  \"zone_count\": " + zoneCount.ToString() + ",");
        FPrintln(file, "  \"enabled_zone_count\": " + enabledCount.ToString() + ",");
        FPrintln(file, "  \"heartbeat\": " + uptime.ToString());
        FPrintln(file, "}");

        CloseFile(file);
    }
}
