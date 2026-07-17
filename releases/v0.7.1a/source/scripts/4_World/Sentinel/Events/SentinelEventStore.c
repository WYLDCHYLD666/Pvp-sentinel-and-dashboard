class SentinelEventStore
{
    static void Init()
    {
        FileHandle file = OpenFile(SentinelConstants.EVENTS_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print(SentinelConstants.LOG_PREFIX + " events.json open failed");
            return;
        }

        FPrintln(file, "{");
        FPrintln(file, "  \"schema_version\": 1,");
        FPrintln(file, "  \"count\": 0,");
        FPrintln(file, "  \"events\": []");
        FPrintln(file, "}");

        CloseFile(file);
        Print(SentinelConstants.LOG_PREFIX + " event store initialized");
    }
}
