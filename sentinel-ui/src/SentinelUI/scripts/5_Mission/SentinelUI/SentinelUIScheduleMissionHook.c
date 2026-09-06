modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        SentinelUIScheduleBridge.GetInstance().Init();
    }
}
