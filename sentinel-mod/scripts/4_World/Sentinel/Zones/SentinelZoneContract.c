class SentinelZoneDefinition
{
    string id;
    string name;
    string type;
    string shape;
    bool enabled;
    int priority;
    string entry_message;
    string exit_message;
    ref array<float> centre;
    float radius;
    ref array<ref array<float>> points;

    void SentinelZoneDefinition()
    {
        enabled = true;
        priority = 100;
        radius = 0;
        centre = new array<float>();
        points = new array<ref array<float>>();
    }
}

class SentinelZoneContract
{
    int schema;
    string contract;
    int revision;
    string world;
    int world_size;
    string generated_at;
    ref array<ref SentinelZoneDefinition> zones;
    string content_hash;

    void SentinelZoneContract()
    {
        schema = 0;
        revision = 0;
        world_size = 0;
        zones = new array<ref SentinelZoneDefinition>();
    }
}

class SentinelLegacyZoneDefinition
{
    string name;
    float x;
    float z;
    float radius;
    bool enabled;

    void SentinelLegacyZoneDefinition()
    {
        radius = 500;
        enabled = false;
    }
}
