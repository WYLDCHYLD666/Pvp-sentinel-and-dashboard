class CfgPatches
{
    class Sentinel
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class Sentinel
    {
        dir = "Sentinel";
        name = "Sentinel";
        action = "https://sentineladmin.co.uk";
        author = "Sentinel Project";
        version = "0.6.2";
        type = "mod";
        dependencies[] = {"World", "Mission"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"Sentinel/scripts/4_World/Sentinel"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"Sentinel/scripts/5_Mission/Sentinel"};
            };
        };
    };
};
