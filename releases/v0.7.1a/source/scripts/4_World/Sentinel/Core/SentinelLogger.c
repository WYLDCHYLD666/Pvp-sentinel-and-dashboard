class SentinelLogger
{
    static void Init()
    {
        Print(SentinelConstants.LOG_PREFIX + " logger init entered " + SentinelVersion.BUILD_ID);

        SentinelFileUtils.EnsureProfileDirectory();
        SentinelBuildInfo.Write();
        SentinelStatusExporter.Write("SAFE", 0, false);
        SentinelEventStore.Init();
        WriteEmptyPlayers();

        Print(SentinelConstants.LOG_PREFIX + " logger initialized " + SentinelVersion.BUILD_ID);
    }

    static void StartPlayerScanTimer()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SentinelLogger.ScanPlayersOnce, 15000, true);
        Print(SentinelConstants.LOG_PREFIX + " player scan timer started");
    }

    static void ScanPlayersOnce()
    {
        ref array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        int playerCount = players.Count();

        SentinelPlayerExporter.Write(players);
        SentinelStatusExporter.Write("SAFE", playerCount, false);

        Print(SentinelConstants.LOG_PREFIX + " live player count = " + playerCount.ToString());
    }

    private static void WriteEmptyPlayers()
    {
        FileHandle file = OpenFile(SentinelConstants.PLAYERS_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print(SentinelConstants.LOG_PREFIX + " players.json initialization failed");
            return;
        }

        FPrintln(file, "{");
        FPrintln(file, "  \"count\": 0,");
        FPrintln(file, "  \"players\": []");
        FPrintln(file, "}");

        CloseFile(file);
    }
}
