modded class PlayerBase
{
    protected string m_SentinelLastDamageAmmo = "";
    protected string m_SentinelLastDamageSourceType = "";
    protected int m_SentinelLastDamageType = -1;
    protected int m_SentinelLastDamageTime = 0;

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        if (GetGame() && GetGame().IsServer())
        {
            m_SentinelLastDamageAmmo = ammo;
            m_SentinelLastDamageType = damageType;
            m_SentinelLastDamageTime = GetGame().GetTime();

            if (source)
            {
                m_SentinelLastDamageSourceType = source.GetType();
            }
            else
            {
                m_SentinelLastDamageSourceType = "";
            }
        }

        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
    }

    protected PlayerBase SentinelResolveKillerPlayer(Object killer, out string resolution)
    {
        resolution = "none";

        if (!killer)
        {
            resolution = "null killer";
            return null;
        }

        PlayerBase directPlayer = PlayerBase.Cast(killer);
        if (directPlayer)
        {
            resolution = "direct PlayerBase";
            return directPlayer;
        }

        EntityAI killerEntity = EntityAI.Cast(killer);
        if (killerEntity)
        {
            Man rootMan = killerEntity.GetHierarchyRootPlayer();
            PlayerBase rootPlayer = PlayerBase.Cast(rootMan);

            if (rootPlayer)
            {
                resolution = "EntityAI hierarchy root";
                return rootPlayer;
            }

            resolution = "EntityAI without player hierarchy root";
            return null;
        }

        resolution = "unsupported killer object";
        return null;
    }

    protected string SentinelResolveWeaponDisplayName(string weaponClass)
    {
        if (weaponClass == "" || !GetGame())
        {
            return "";
        }

        string displayName = "";
        string configPath = "CfgWeapons " + weaponClass + " displayName";

        if (GetGame().ConfigIsExisting(configPath))
        {
            GetGame().ConfigGetText(configPath, displayName);
        }

        if (displayName == "")
        {
            displayName = weaponClass;
        }

        return displayName;
    }

    protected string SentinelClassifyPvPCause(Object killer)
    {
        if (Weapon_Base.Cast(killer))
        {
            return "firearm";
        }

        if (ItemBase.Cast(killer))
        {
            return "melee";
        }

        if (PlayerBase.Cast(killer))
        {
            return "player";
        }

        return "unknown";
    }

    protected bool SentinelContains(string value, string token)
    {
        if (value == "" || token == "")
        {
            return false;
        }

        string loweredValue = value;
        string loweredToken = token;
        loweredValue.ToLower();
        loweredToken.ToLower();
        return loweredValue.IndexOf(loweredToken) >= 0;
    }

    protected string SentinelClassifyEnvironmentalCause(Object killer, float energy, float water, float blood, int chemicalAgentCount)
    {
        if (ZombieBase.Cast(killer) || SentinelContains(m_SentinelLastDamageSourceType, "zmb") || SentinelContains(m_SentinelLastDamageSourceType, "infected"))
        {
            return "infected";
        }

        // Stat depletion can arrive with the victim itself as the killer.
        // Evaluate the vital state before self-kill fallback.
        if (energy <= 0.0)
        {
            return "starvation";
        }

        if (water <= 0.0)
        {
            return "dehydration";
        }

        if (blood <= 0.0)
        {
            return "bleeding";
        }

        // Chemical poisoning is tracked by the player agent system and may not
        // invoke EEHitBy with a usable ammo/source value. Check the active
        // chemical agent before relying on cached damage context.
        if (chemicalAgentCount > 0)
        {
            return "gas";
        }

        // Use the most recent damage context for non-player environmental deaths.
        if (SentinelContains(m_SentinelLastDamageAmmo, "fall") || SentinelContains(m_SentinelLastDamageSourceType, "fall"))
        {
            return "fall_damage";
        }

        if (SentinelContains(m_SentinelLastDamageAmmo, "drown") || SentinelContains(m_SentinelLastDamageSourceType, "drown"))
        {
            return "drowning";
        }

        if (SentinelContains(m_SentinelLastDamageAmmo, "contaminated") || SentinelContains(m_SentinelLastDamageAmmo, "gas") || SentinelContains(m_SentinelLastDamageSourceType, "contaminated"))
        {
            return "gas";
        }

        if (SentinelContains(m_SentinelLastDamageAmmo, "grenade") || SentinelContains(m_SentinelLastDamageAmmo, "explosion") || SentinelContains(m_SentinelLastDamageAmmo, "explosive") || SentinelContains(m_SentinelLastDamageSourceType, "grenade") || SentinelContains(m_SentinelLastDamageSourceType, "explosive"))
        {
            return "explosion";
        }

        if (Transport.Cast(killer) || SentinelContains(m_SentinelLastDamageSourceType, "car") || SentinelContains(m_SentinelLastDamageSourceType, "vehicle"))
        {
            return "vehicle";
        }

        // DayZ frequently supplies the victim as the killer for environmental
        // deaths. A self reference alone is not proof of suicide.
        return "unknown";
    }

    override void EEKilled(Object killer)
    {
        bool isServer = GetGame() && GetGame().IsServer();

        string victimName = "";
        string victimId = "";
        string killerName = "";
        string killerId = "";
        string killerType = "null";
        string killerResolution = "not evaluated";
        string weaponClass = "";
        string weaponDisplayName = "";
        string cause = "unknown";
        float distanceMetres = 0.0;
        float victimEnergy = GetStatEnergy().Get();
        float victimWater = GetStatWater().Get();
        float victimBlood = GetHealth("", "Blood");
        float victimHealth = GetHealth("", "Health");
        int victimChemicalAgentCount = GetSingleAgentCount(eAgents.CHEMICAL_POISON);
        vector victimPosition = GetPosition();
        vector killerPosition = "0 0 0";

        if (isServer)
        {
            PlayerIdentity victimIdentity = GetIdentity();

            if (victimIdentity)
            {
                victimName = victimIdentity.GetName();
                victimId = victimIdentity.GetId();
            }

            if (killer)
            {
                killerType = killer.GetType();
                cause = SentinelClassifyPvPCause(killer);

                if (EntityAI.Cast(killer))
                {
                    weaponClass = killerType;
                    weaponDisplayName = SentinelResolveWeaponDisplayName(weaponClass);
                }
            }

            PlayerBase killerPlayer = SentinelResolveKillerPlayer(killer, killerResolution);

            if (!killerPlayer || killerPlayer == this)
            {
                cause = SentinelClassifyEnvironmentalCause(killer, victimEnergy, victimWater, victimBlood, victimChemicalAgentCount);
            }

            if (killerPlayer && killerPlayer != this)
            {
                killerPosition = killerPlayer.GetPosition();
                distanceMetres = vector.Distance(killerPosition, victimPosition);

                PlayerIdentity killerIdentity = killerPlayer.GetIdentity();

                if (killerIdentity)
                {
                    killerName = killerIdentity.GetName();
                    killerId = killerIdentity.GetId();
                }
                else
                {
                    killerResolution = killerResolution + "; player identity unavailable";
                }
            }
            else if (killerPlayer == this)
            {
                killerResolution = killerResolution + "; self kill";
            }

            Print(SentinelConstants.LOG_PREFIX + " death snapshot victim=" + victimName + " (" + victimId + ") killer_type=" + killerType + " resolution=" + killerResolution + " killer=" + killerName + " (" + killerId + ") cause=" + cause + " weapon=" + weaponClass + " distance_m=" + distanceMetres.ToString() + " energy=" + victimEnergy.ToString() + " water=" + victimWater.ToString() + " blood=" + victimBlood.ToString() + " health=" + victimHealth.ToString() + " chemical_agent=" + victimChemicalAgentCount.ToString() + " last_damage_ammo=" + m_SentinelLastDamageAmmo + " last_damage_source=" + m_SentinelLastDamageSourceType + " last_damage_type=" + m_SentinelLastDamageType.ToString() + " last_damage_age_ms=" + (GetGame().GetTime() - m_SentinelLastDamageTime).ToString());
        }

        super.EEKilled(killer);

        if (!isServer)
        {
            return;
        }

        SentinelEventStore.RecordPlayerDeathSnapshot(victimName, victimId, killerName, killerId, weaponClass, weaponDisplayName, cause, distanceMetres, killerPosition, victimPosition);
    }
};
