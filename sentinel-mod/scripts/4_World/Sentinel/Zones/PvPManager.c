class PvPManager
{
    static bool IsWeekend()
    {
        return false;
    }

    static bool IsPositionInPvPZone(vector position)
    {
        return SentinelPvPZoneManager.GetInstance().IsPositionInPvPZone(position);
    }

    static SentinelZoneDefinition GetWinningZone(vector position)
    {
        return SentinelPvPZoneManager.GetInstance().GetWinningZone(position);
    }

    static bool ArePositionsInSamePvPZone(vector firstPosition, vector secondPosition)
    {
        SentinelZoneDefinition firstZone;
        SentinelZoneDefinition secondZone;

        firstZone = SentinelPvPZoneManager.GetInstance().GetWinningZone(firstPosition);
        secondZone = SentinelPvPZoneManager.GetInstance().GetWinningZone(secondPosition);

        if (!firstZone)
        {
            return false;
        }

        if (!secondZone)
        {
            return false;
        }

        if (firstZone.type != "pvp")
        {
            return false;
        }

        if (secondZone.type != "pvp")
        {
            return false;
        }

        return firstZone.id == secondZone.id;
    }

    static bool IsCombatAllowedForPlayers(PlayerBase attacker, PlayerBase victim)
    {
        if (IsWeekend())
        {
            return true;
        }

        if (!attacker)
        {
            return false;
        }

        if (!victim)
        {
            return false;
        }

        vector attackerPos;
        vector victimPos;

        attackerPos = attacker.GetPosition();
        victimPos = victim.GetPosition();

        return ArePositionsInSamePvPZone(attackerPos, victimPos);
    }
}
