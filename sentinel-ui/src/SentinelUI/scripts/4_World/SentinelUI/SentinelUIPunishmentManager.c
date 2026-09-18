class SentinelUIPunishmentManager
{
    static const int WARNING_COOLDOWN_MS = 1500;
    protected static ref map<string, int> s_WarningCounts = new map<string, int>();
    protected static ref map<string, int> s_LastWarningTimes = new map<string, int>();

    static bool ShouldBlockDamage(PlayerBase attacker, PlayerBase victim, string ammo)
    {
        if (!SentinelUIEnforcementState.ArePunishmentsEnabled())
        {
            return false;
        }

        bool legalDamage;
        string mode = SentinelUIEnforcementState.GetPvPMode();

#ifdef Sentinel
        if (mode == "WHOLE_MAP_KOS")
        {
            legalDamage = true;
        }
        else if (mode == "ZONE_ONLY")
        {
            legalDamage = SentinelPvPZoneManager.GetInstance().ArePositionsInSamePvPZone(attacker.GetPosition(), victim.GetPosition());
        }
#endif

        if (legalDamage)
        {
            return false;
        }

        RegisterWarning(attacker, victim, mode, ammo);
        return true;
    }

    protected static void RegisterWarning(PlayerBase attacker, PlayerBase victim, string mode, string ammo)
    {
        PlayerIdentity attackerIdentity = attacker.GetIdentity();

        if (!attackerIdentity)
        {
            return;
        }

        string uid = attackerIdentity.GetId();
        int now = GetGame().GetTime();
        int lastWarning;

        if (s_LastWarningTimes.Contains(uid))
        {
            lastWarning = s_LastWarningTimes.Get(uid);
        }

        if ((now - lastWarning) < WARNING_COOLDOWN_MS)
        {
            return;
        }

        int count;

        if (s_WarningCounts.Contains(uid))
        {
            count = s_WarningCounts.Get(uid);
        }

        count++;
        s_WarningCounts.Set(uid, count);
        s_LastWarningTimes.Set(uid, now);

        string message = "SHOOTING OUTSIDE A PVP ZONE";

        if (mode == "SAFE")
        {
            message = "PVP IS DISABLED IN SAFE MODE";
        }

        Param2<int, string> payload = new Param2<int, string>(count, message);
        GetGame().RPCSingleParam(attacker, SentinelUIZoneProtocol.RPC_PUNISHMENT_WARNING, payload, true, attackerIdentity);

        string victimName = "unknown";
        if (victim.GetIdentity())
        {
            victimName = victim.GetIdentity().GetName();
        }

        Print("[SentinelUI] blocked illegal PvP warning=" + count.ToString() + " attacker=" + attackerIdentity.GetName() + " victim=" + victimName + " mode=" + mode + " ammo=" + ammo);
    }
}
