class SentinelSessionManager
{
    private static ref map<string, int> s_PlayerSessionStart = new map<string, int>();

    static int GetOnlineTime(string playerId)
    {
        int nowSeconds = GetGame().GetTime() / 1000;

        if (!s_PlayerSessionStart.Contains(playerId))
        {
            s_PlayerSessionStart.Insert(playerId, nowSeconds);
            Print("[Sentinel] started player session = " + playerId);
        }

        return nowSeconds - s_PlayerSessionStart.Get(playerId);
    }

    static void CleanupDisconnectedPlayers(array<Man> players)
    {
        ref map<string, bool> connectedPlayerIds = new map<string, bool>();
        ref array<string> disconnectedPlayerIds = new array<string>();

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (!player) continue;

            PlayerIdentity identity = player.GetIdentity();
            if (!identity) continue;

            connectedPlayerIds.Insert(identity.GetId(), true);
        }

        foreach (string sessionPlayerId, int sessionStart : s_PlayerSessionStart)
        {
            if (!connectedPlayerIds.Contains(sessionPlayerId))
            {
                disconnectedPlayerIds.Insert(sessionPlayerId);
            }
        }

        foreach (string disconnectedPlayerId : disconnectedPlayerIds)
        {
            s_PlayerSessionStart.Remove(disconnectedPlayerId);
            Print("[Sentinel] cleared disconnected player session = " + disconnectedPlayerId);
        }
    }
}
