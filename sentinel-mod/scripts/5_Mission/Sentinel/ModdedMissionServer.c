modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();

        Print(SentinelConstants.LOG_PREFIX + " ==================================================");
        Print(SentinelConstants.LOG_PREFIX + " loading " + SentinelVersion.DISPLAY_NAME + " " + SentinelVersion.BUILD_ID);
        Print(SentinelConstants.LOG_PREFIX + " server profile target: " + SentinelConstants.PROFILE_DIR);

        SentinelLogger.Init();
        SentinelPvPZoneManager.GetInstance().Init();
        SentinelLogger.ScanPlayersOnce();
        SentinelLogger.StartPlayerScanTimer();

        Print(SentinelConstants.LOG_PREFIX + " MissionServer initialized " + SentinelVersion.BUILD_ID);
        Print(SentinelConstants.LOG_PREFIX + " ==================================================");
    }
}
