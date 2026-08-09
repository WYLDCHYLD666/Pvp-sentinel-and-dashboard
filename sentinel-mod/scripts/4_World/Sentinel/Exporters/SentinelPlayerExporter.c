class SentinelPlayerExporter
{
    static void Write(array<Man> players)
    {
        SentinelSessionManager.CleanupDisconnectedPlayers(players);

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

            string playerId = identity.GetId();
            int onlineTime = SentinelSessionManager.GetOnlineTime(playerId);

            if (writtenPlayers > 0)
            {
                FPrintln(file, "    ,");
            }

            vector position = player.GetPosition();
            vector orientation = player.GetOrientation();
            float health = player.GetHealth("", "Health");
            float blood = player.GetHealth("", "Blood");
            float shock = player.GetHealth("", "Shock");

            FPrintln(file, "    {");
            FPrintln(file, "      \"name\": \"" + identity.GetName() + "\",");
            FPrintln(file, "      \"id\": \"" + playerId + "\",");
            FPrintln(file, "      \"position\": {");
            FPrintln(file, "        \"x\": " + position[0].ToString() + ",");
            FPrintln(file, "        \"y\": " + position[1].ToString() + ",");
            FPrintln(file, "        \"z\": " + position[2].ToString());
            FPrintln(file, "      },");
            FPrintln(file, "      \"heading\": " + orientation[0].ToString() + ",");
            FPrintln(file, "      \"health\": " + health.ToString() + ",");
            FPrintln(file, "      \"blood\": " + blood.ToString() + ",");
            FPrintln(file, "      \"shock\": " + shock.ToString() + ",");
            FPrintln(file, "      \"onlineTime\": " + onlineTime.ToString());
            FPrintln(file, "    }");

            writtenPlayers++;
        }

        FPrintln(file, "  ]");
        FPrintln(file, "}");

        CloseFile(file);
        Print("[Sentinel] live player records written = " + writtenPlayers.ToString());
    }
}
