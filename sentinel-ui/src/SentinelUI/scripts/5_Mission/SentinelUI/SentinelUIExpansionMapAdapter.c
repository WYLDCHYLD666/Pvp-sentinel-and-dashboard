modded class ExpansionMapMenu
{
    protected ref SentinelUIMapZoneOverlay m_SentinelUIExpansionMapZoneOverlay;

    override Widget Init()
    {
        Widget root = super.Init();

        if (m_SentinelUIExpansionMapZoneOverlay)
        {
            m_SentinelUIExpansionMapZoneOverlay.Destroy();
            m_SentinelUIExpansionMapZoneOverlay = null;
        }

        MapWidget expansionMapWidget = GetMap();

        if (expansionMapWidget)
        {
            m_SentinelUIExpansionMapZoneOverlay = new SentinelUIMapZoneOverlay();

                if (!m_SentinelUIExpansionMapZoneOverlay.Init(expansionMapWidget, root))
            {
                m_SentinelUIExpansionMapZoneOverlay = null;
            }
            else
            {
                Print("[SentinelUI] Expansion map-zone adapter ready");
            }
        }

        return root;
    }

    override void OnShow()
    {
        super.OnShow();
        SentinelUIMapZoneStore.RequestFromServer();
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);

        if (m_SentinelUIExpansionMapZoneOverlay)
        {
            m_SentinelUIExpansionMapZoneOverlay.Update();
        }
    }
}
