class SentinelBuildInfo
{
    static void Write()
    {
        FileHandle file = OpenFile(SentinelConstants.BUILD_INFO_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print(SentinelConstants.LOG_PREFIX + " build_info.json open failed");
            return;
        }

        FPrintln(file, "{");
        FPrintln(file, "  \"name\": \"" + SentinelVersion.DISPLAY_NAME + "\",");
        FPrintln(file, "  \"version\": \"" + SentinelVersion.VERSION + "\",");
        FPrintln(file, "  \"build_id\": \"" + SentinelVersion.BUILD_ID + "\",");
        FPrintln(file, "  \"server_only\": true");
        FPrintln(file, "}");

        CloseFile(file);
    }
}
