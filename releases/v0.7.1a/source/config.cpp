class CfgPatches
{
    class Sentinel
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
    class Sentinel
    {
        dir = "Sentinel";
        name = "Sentinel";
        picture = "";
        action = "https://sentineladmin.co.uk";
        hideName = 0;
        hidePicture = 0;
        credits = "Sentinel Project";
        author = "Sentinel Project";
        authorID = "0";
        version = "0.7.1";
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
                    "Sentinel/scripts/4_World/Sentinel"
                };
            };

            class missionScriptModule
            {
                value = "";
                files[] =
                {
                    "Sentinel/scripts/5_Mission/Sentinel"
                };
            };
        };
    };
};
