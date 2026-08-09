class SentinelConstants
{
    static const string LOG_PREFIX = "[Sentinel]";

    static const string PROFILE_DIR = "$profile:Sentinel";
    static const string BUILD_INFO_FILE = "$profile:Sentinel/build_info.json";
    static const string STATUS_FILE = "$profile:Sentinel/status.json";
    static const string EVENTS_FILE = "$profile:Sentinel/events.json";
    static const string ZONES_FILE = "$profile:Sentinel/zones.json";
    static const string PLAYERS_FILE = "$profile:Sentinel/players.json";
    static const string ZONE_STATUS_FILE = "$profile:Sentinel/zone_status.json";

    static const string ZONE_CONTRACT_NAME = "sentinel.zone-config";
    static const int ZONE_RELOAD_INTERVAL_MS = 30000;
    static const int ZONE_PLAYER_SCAN_INTERVAL_MS = 5000;

    static const string FALLBACK_FILE = "$profile:Sentinel_build_info.json";
}
