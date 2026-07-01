class PvPPunishment
{
    protected ref map<string, int> m_Violations;

    void PvPPunishment()
    {
        m_Violations = new map<string, int>();
        Print(PvPUltimateConstants.LOG_PREFIX + " punishment system initialized");
    }

    void RegisterViolation(PlayerBase player)
    {
        if (!GetGame() || !GetGame().IsServer())
        {
            return;
        }

        if (!player)
        {
            return;
        }

        PlayerIdentity identity = player.GetIdentity();

        if (!identity)
        {
            Print(PvPUltimateConstants.LOG_PREFIX + " illegal PvP attacker identity missing");
            return;
        }

        string uid = identity.GetId();

        if (uid == string.Empty)
        {
            Print(PvPUltimateConstants.LOG_PREFIX + " illegal PvP attacker UID missing");
            return;
        }

        int count = 0;

        if (m_Violations.Contains(uid))
        {
            count = m_Violations.Get(uid);
        }

        count++;
        m_Violations.Set(uid, count);

        if (count == 1)
        {
            Print(PvPUltimateConstants.LOG_PREFIX + " WARNING: illegal PvP by " + identity.GetName());
            return;
        }

        if (count == 2)
        {
            Print(PvPUltimateConstants.LOG_PREFIX + " SIMULATED KICK: illegal PvP by " + identity.GetName());
            return;
        }

        Print(PvPUltimateConstants.LOG_PREFIX + " SIMULATED BAN: illegal PvP by " + identity.GetName());
    }
}
