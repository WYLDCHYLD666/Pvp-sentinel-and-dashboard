class PvPManager
{
    protected static ref PvPPunishment s_Punishment;

    static PvPPunishment GetPunishment()
    {
        if (!s_Punishment)
        {
            s_Punishment = new PvPPunishment();
        }

        return s_Punishment;
    }

    static bool CanDamage(PlayerBase attacker, PlayerBase victim)
    {
        if (!attacker || !victim)
        {
            return false;
        }

        PvPZoneManager zoneManager = PvPZoneManager.GetInstance();

        if (!zoneManager)
        {
            return false;
        }

        if (zoneManager.IsWeekendPvP())
        {
            return true;
        }

        bool attackerInZone = zoneManager.InZone(attacker.GetPosition());
        bool victimInZone = zoneManager.InZone(victim.GetPosition());

        return attackerInZone && victimInZone;
    }

    static bool ShouldBlockDamage(PlayerBase attacker, PlayerBase victim)
    {
        if (!attacker || !victim)
        {
            return false;
        }

        if (attacker == victim)
        {
            return false;
        }

        if (CanDamage(attacker, victim))
        {
            return false;
        }

        GetPunishment().RegisterViolation(attacker);

        Print(PvPUltimateConstants.LOG_PREFIX + " blocked illegal PvP damage");

        return true;
    }
}
