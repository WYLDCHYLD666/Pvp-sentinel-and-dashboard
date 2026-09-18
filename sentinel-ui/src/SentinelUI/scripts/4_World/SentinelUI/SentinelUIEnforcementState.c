class SentinelUIEnforcementState
{
    static const int REFRESH_INTERVAL_MS = 2000;
    static const float REFRESH_RADIUS = 1200.0;

    protected static bool s_Initialized;
    protected static bool s_HasRule;
    protected static bool s_PurgeActive;
    protected static bool s_PunishmentsEnabled;
    protected static string s_PvPMode = "ZONE_ONLY";

    static void ApplyRuleProfile(SentinelUIScheduleProfile profile, bool purgeActive)
    {
        ApplyPurgeState(purgeActive);

        if (profile)
        {
            s_PvPMode = profile.pvp_mode;
            s_PunishmentsEnabled = profile.punishments && !purgeActive;
        }

        Print("[SentinelUI] punishment rule mode=" + s_PvPMode + " enabled=" + s_PunishmentsEnabled.ToString());
    }

    static void ApplyPurgeState(bool purgeActive)
    {
        if (!GetGame() || !GetGame().IsServer())
        {
            return;
        }

        Init();
        bool changed = !s_HasRule || s_PurgeActive != purgeActive;
        s_HasRule = true;
        s_PurgeActive = purgeActive;

        if (purgeActive)
        {
            s_PvPMode = "WHOLE_MAP_KOS";
            s_PunishmentsEnabled = false;
        }
        else if (s_PvPMode == "WHOLE_MAP_KOS")
        {
            s_PvPMode = "ZONE_ONLY";
            s_PunishmentsEnabled = true;
        }

        // Purge: damage enabled. Normal/weekday rules: bases and containers protected.
        CfgGameplayHandler.m_Data.GeneralData.disableBaseDamage = !purgeActive;
        CfgGameplayHandler.m_Data.GeneralData.disableContainerDamage = !purgeActive;

        if (changed)
        {
            int baseCount;
            int containerCount;
            int stateMismatchCount;
            RefreshNearbyObjects(baseCount, containerCount, stateMismatchCount);
            Print("[SentinelUI] enforcement changed purge=" + purgeActive.ToString() + " disableBaseDamage=" + (!purgeActive).ToString() + " disableContainerDamage=" + (!purgeActive).ToString() + " basesUpdated=" + baseCount.ToString() + " containersUpdated=" + containerCount.ToString() + " stateMismatches=" + stateMismatchCount.ToString());
        }
    }

    static bool IsPurgeActive()
    {
        return s_HasRule && s_PurgeActive;
    }

    static bool ArePunishmentsEnabled()
    {
        return s_HasRule && s_PunishmentsEnabled && !s_PurgeActive;
    }

    static string GetPvPMode()
    {
        return s_PvPMode;
    }

    protected static void Init()
    {
        if (s_Initialized)
        {
            return;
        }

        s_Initialized = true;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SentinelUIEnforcementState.RefreshTick, REFRESH_INTERVAL_MS, true);
        Print("[SentinelUI] dynamic base/container enforcement initialized");
    }

    static void RefreshTick()
    {
        if (s_HasRule)
        {
            int baseCount;
            int containerCount;
            int stateMismatchCount;
            RefreshNearbyObjects(baseCount, containerCount, stateMismatchCount);
        }
    }

    protected static void RefreshNearbyObjects(out int baseCount, out int containerCount, out int stateMismatchCount)
    {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            if (!man)
            {
                continue;
            }

            array<Object> objects = new array<Object>();
            array<CargoBase> proxies = new array<CargoBase>();
            GetGame().GetObjectsAtPosition3D(man.GetPosition(), REFRESH_RADIUS, objects, proxies);

            foreach (Object object : objects)
            {
                EntityAI entity = EntityAI.Cast(object);

                if (!entity)
                {
                    continue;
                }

                string invulnerabilityType = entity.GetInvulnerabilityTypeString();

                if (invulnerabilityType == "disableBaseDamage" || invulnerabilityType == "disableContainerDamage")
                {
                    entity.SetAllowDamage(s_PurgeActive);

                    if (invulnerabilityType == "disableBaseDamage")
                    {
                        baseCount++;
                    }
                    else
                    {
                        containerCount++;
                    }

                    if (entity.GetAllowDamage() != s_PurgeActive)
                    {
                        stateMismatchCount++;
                    }
                }
            }
        }
    }

    static bool EvaluateDamage(EntityAI target, bool upstreamAllowed, string ammo)
    {
        if (!GetGame() || !GetGame().IsServer() || !target || !s_HasRule)
        {
            return upstreamAllowed;
        }

        string invulnerabilityType = target.GetInvulnerabilityTypeString();

        if (invulnerabilityType != "disableBaseDamage" && invulnerabilityType != "disableContainerDamage")
        {
            return upstreamAllowed;
        }

        bool allowed = s_PurgeActive && upstreamAllowed;
        Print("[SentinelUI] damage attempt target=" + target.GetType() + " category=" + invulnerabilityType + " purge=" + s_PurgeActive.ToString() + " allowDamage=" + target.GetAllowDamage().ToString() + " upstreamAllowed=" + upstreamAllowed.ToString() + " result=" + allowed.ToString() + " ammo=" + ammo);
        return allowed;
    }
}
