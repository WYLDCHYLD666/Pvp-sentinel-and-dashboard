modded class PlayerBase
{
    override bool EEOnDamageCalculated(
        TotalDamageResult damageResult,
        int damageType,
        EntityAI source,
        int component,
        string dmgZone,
        string ammo,
        vector modelPos,
        float speedCoef
    )
    {
        if (!super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef))
        {
            return false;
        }

        if (!GetGame() || !GetGame().IsServer())
        {
            return true;
        }

        if (!source)
        {
            return true;
        }

        PlayerBase attacker;

        if (!Class.CastTo(attacker, source.GetHierarchyRootPlayer()))
        {
            return true;
        }

        if (PvPManager.ShouldBlockDamage(attacker, this))
        {
            return false;
        }

        return true;
    }
}
