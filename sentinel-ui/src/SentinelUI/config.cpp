class CfgPatches
{
    class SentinelUI
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Sounds_Effects", "DayZExpansion_Navigation_Scripts"};
    };
};

class CfgMods
{
    class SentinelUI
    {
        dir = "SentinelUI";
        name = "Sentinel UI";
        author = "Sentinel Project";
        version = "0.3.0-purge-scheduler-alpha8-complete-zone-cards";
        type = "mod";
        dependencies[] = {"World", "Mission"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"SentinelUI/scripts/4_World/SentinelUI"};
            };

            class missionScriptModule
            {
                value = "";
                files[] = {"SentinelUI/scripts/5_Mission/SentinelUI"};
            };
        };
    };
};

class CfgSoundShaders
{
    class SentinelUI_Purge_SoundShader
    {
        samples[] = {{"SentinelUI\\sounds\\purge_siren_pcm", 1}};
        volume = 1;
        range = 50;
        limitation = 0;
    };
};

class CfgSoundSets
{
    class SentinelUI_Purge_SoundSet
    {
        soundShaders[] = {"SentinelUI_Purge_SoundShader"};
        volumeFactor = 1;
        frequencyFactor = 1;
        spatial = 0;
    };
};
