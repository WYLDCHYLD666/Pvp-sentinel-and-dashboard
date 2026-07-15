class SentinelLogger
{
    static void Init()
    {
        Print("[Sentinel] logger init entered v0.6.2.7");

        EnsureProfileDirectory();
        WriteBuildInfo();
        WriteStatus("SAFE", 0, false);
        WriteEmptyArray(SentinelConstants.EVENTS_FILE);
        WriteEmptyArray(SentinelConstants.PLAYERS_FILE);
        WriteFallbackBuildInfo();

        Print("[Sentinel] logger initialized v0.6.2.7-live-player-position");
    }

    static void EnsureProfileDirectory()
    {
        if (!FileExist(SentinelConstants.PROFILE_DIR))
        {
            MakeDirectory(SentinelConstants.PROFILE_DIR);
        }
    }

    static void WriteBuildInfo()
    {
        FileHandle file = OpenFile(SentinelConstants.BUILD_INFO_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print("[Sentinel] build_info.json open failed");
            return;
        }

        FPrintln(file, "{");
        FPrintln(file, "  \"version\": \"0.6.2.3b\",");
        FPrintln(file, "  \"build_id\": \"v0.6.2.7-live-player-position\",");
        FPrintln(file, "  \"server_only\": true");
        FPrintln(file, "}");

        CloseFile(file);
    }

    static void WriteStatus(string mode, int onlinePlayers, bool weekendPvP)
    {
        FileHandle file = OpenFile(SentinelConstants.STATUS_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print("[Sentinel] status.json open failed");
            return;
        }

        FPrintln(file, "{");
        FPrintln(file, "  \"version\": \"0.6.2.3b\",");
        FPrintln(file, "  \"mode\": \"" + mode + "\",");
        FPrintln(file, "  \"online_players\": " + onlinePlayers.ToString() + ",");
        FPrintln(file, "  \"weekend_pvp\": " + BoolToJson(weekendPvP));
        FPrintln(file, "}");

        CloseFile(file);
    }

    static void WriteEmptyArray(string path)
    {
        FileHandle file = OpenFile(path, FileMode.WRITE);

        if (file == 0)
        {
            Print("[Sentinel] empty file open failed");
            return;
        }

        if (path == SentinelConstants.PLAYERS_FILE)
        {
            FPrintln(file, "{");
            FPrintln(file, "  \"count\": 0,");
            FPrintln(file, "  \"players\": []");
            FPrintln(file, "}");
        }
        else
        {
            FPrintln(file, "[]");
        }

        CloseFile(file);
    }

    static void WriteFallbackBuildInfo()
    {
        FileHandle file = OpenFile(SentinelConstants.FALLBACK_FILE, FileMode.WRITE);

        if (file == 0)
        {
            return;
        }

        FPrintln(file, "{");
        FPrintln(file, "  \"build_id\": \"v0.6.2.7-live-player-position\"");
        FPrintln(file, "}");

        CloseFile(file);
    }

    static void StartPlayerScanTimer()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SentinelLogger.ScanPlayersOnce, 15000, true);
        Print("[Sentinel] player scan timer started");
    }

    static void ScanPlayersOnce()
    {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        int playerCount = players.Count();

        WriteLivePlayers(players);
        WriteStatus("SAFE", playerCount, false);

        Print("[Sentinel] live player count = " + playerCount.ToString());
    }

    static void WriteLivePlayers(array<Man> players)
    {
        FileHandle file = OpenFile(SentinelConstants.PLAYERS_FILE, FileMode.WRITE);

        if (file == 0)
        {
            Print("[Sentinel] players.json open failed");
            return;
        }

        FPrintln(file, "{");
        FPrintln(file, "  \"count\": " + players.Count().ToString() + ",");
        FPrintln(file, "  \"players\": [");

        int writtenPlayers = 0;

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);

            if (!player)
            {
                continue;
            }

            PlayerIdentity identity = player.GetIdentity();

            if (!identity)
            {
                continue;
            }

            if (writtenPlayers > 0)
            {
                FPrintln(file, "    ,");
            }

            vector position = player.GetPosition();

            FPrintln(file, "    {");
            FPrintln(file, "      \"name\": \"" + identity.GetName() + "\",");
            FPrintln(file, "      \"id\": \"" + identity.GetId() + "\",");
            FPrintln(file, "      \"position\": {");
            FPrintln(file, "        \"x\": " + position[0].ToString() + ",");
            FPrintln(file, "        \"y\": " + position[1].ToString() + ",");
            FPrintln(file, "        \"z\": " + position[2].ToString());
            FPrintln(file, "      }");
            FPrintln(file, "    }");

            writtenPlayers++;
        }

        FPrintln(file, "  ]");
        FPrintln(file, "}");

        CloseFile(file);

        Print("[Sentinel] live player records written = " + writtenPlayers.ToString());
    }

    static string BoolToJson(bool value)
    {
        if (value)
        {
            return "true";
        }

        return "false";
    }
}
