class CfgPatches
{
    class PvPUltimate
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] =
        {
            "DZ_Data",
            "DZ_Scripts"
        };
    };
};

class CfgMods
{
    class PvPUltimate
    {
        dir = "PvPUltimate";
        name = "PvPUltimate";
        type = "mod";

        dependencies[] =
        {
            "World",
            "Mission"
        };

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] =
                {
                    "pvpultimate/scripts/4_World/PvPUltimate"
                };
            };

            class missionScriptModule
            {
                value = "";
                files[] =
                {
                    "pvpultimate/scripts/5_Mission/PvPUltimate"
                };
            };
        };
    };
};
