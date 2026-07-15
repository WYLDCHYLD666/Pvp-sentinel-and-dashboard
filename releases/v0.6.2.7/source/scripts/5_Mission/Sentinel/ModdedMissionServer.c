modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();

        SentinelLogger.Init();
        SentinelPvPZoneManager.GetInstance().Init();
        SentinelLogger.ScanPlayersOnce();
        SentinelLogger.StartPlayerScanTimer();

        Print("[Sentinel] MissionServer initialized v0.6.2.7-live-player-position");
    }
};
