modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();

        PvPZoneManager.GetInstance().Init();

        Print(PvPUltimateConstants.LOG_PREFIX + " MissionServer initialized with EEOnDamageCalculated hook");
    }
}
