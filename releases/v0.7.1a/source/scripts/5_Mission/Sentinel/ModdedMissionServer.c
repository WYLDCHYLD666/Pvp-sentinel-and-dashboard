modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();

        SentinelLogger.Init();
        SentinelPvPZoneManager.GetInstance().Init();
        SentinelLogger.ScanPlayersOnce();
        SentinelLogger.StartPlayerScanTimer();

        Print(SentinelConstants.LOG_PREFIX + " MissionServer initialized " + SentinelVersion.BUILD_ID);
    }
};
