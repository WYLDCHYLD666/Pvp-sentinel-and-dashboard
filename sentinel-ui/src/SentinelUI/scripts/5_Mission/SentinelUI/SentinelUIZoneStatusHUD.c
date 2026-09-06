class SentinelUIZoneStatusHUD
{
    protected Widget m_Root;
    protected Widget m_Panel;
    protected ImageWidget m_Icon;
    protected TextWidget m_Status;
    protected TextWidget m_Zone;

    bool Init()
    {
        if (!GetGame() || !GetGame().GetWorkspace())
        {
            return false;
        }

        m_Root = GetGame().GetWorkspace().CreateWidgets("SentinelUI/gui/layouts/sentinel_zone_status.layout");

        if (!m_Root)
        {
            Print("[SentinelUI] failed to create zone status layout");
            return false;
        }

        m_Panel = m_Root.FindAnyWidget("SentinelUIZoneStatusPanel");
        m_Icon = ImageWidget.Cast(m_Root.FindAnyWidget("SentinelUIZoneStatusIcon"));
        m_Status = TextWidget.Cast(m_Root.FindAnyWidget("SentinelUIZoneStatusLabel"));
        m_Zone = TextWidget.Cast(m_Root.FindAnyWidget("SentinelUIZoneStatusZone"));

        if (m_Icon)
        {
            m_Icon.LoadImageFile(0, "SentinelUI/gui/icons/safe_ca.paa");
            m_Icon.LoadImageFile(1, "SentinelUI/gui/icons/pvp_ca.paa");
            m_Icon.LoadImageFile(2, "SentinelUI/gui/icons/purge_ca.paa");
            m_Icon.LoadImageFile(3, "SentinelUI/gui/icons/trader_ca.paa");
            m_Icon.LoadImageFile(4, "SentinelUI/gui/icons/admin_ca.paa");
            m_Icon.LoadImageFile(5, "SentinelUI/gui/icons/event_ca.paa");
            m_Icon.LoadImageFile(6, "SentinelUI/gui/icons/radiation_ca.paa");
            m_Icon.LoadImageFile(7, "SentinelUI/gui/icons/raid_ca.paa");
        }

        SetStatus("safe", "", false);
        m_Root.Show(true);
        return true;
    }

    void Destroy()
    {
        if (m_Root)
        {
            m_Root.Unlink();
        }

        m_Root = null;
        m_Panel = null;
        m_Icon = null;
        m_Status = null;
        m_Zone = null;
    }

    void SetStatus(string zoneType, string zoneName, bool purgeActive)
    {
        if (purgeActive)
        {
            if (m_Panel)
            {
                m_Panel.SetColor(ARGB(235, 66, 3, 7));
            }

            if (m_Icon)
            {
                m_Icon.SetImage(2);
            }

            if (m_Status)
            {
                m_Status.SetText("PURGE");
                m_Status.SetColor(ARGB(255, 255, 55, 55));
            }

            if (m_Zone)
            {
                m_Zone.SetText("MAP-WIDE PVP");
            }
        }
        else if (zoneType == "pvp")
        {
            if (m_Panel)
            {
                m_Panel.SetColor(ARGB(230, 74, 13, 20));
            }

            if (m_Icon)
            {
                m_Icon.SetImage(1);
            }

            if (m_Status)
            {
                m_Status.SetText("PVP");
                m_Status.SetColor(ARGB(255, 255, 82, 82));
            }

            if (m_Zone)
            {
                if (zoneName == "")
                {
                    m_Zone.SetText("PVP ZONE");
                }
                else
                {
                    m_Zone.SetText(zoneName);
                }
            }
        }
        else if (zoneType == "trader")
        {
            if (m_Panel)
            {
                m_Panel.SetColor(ARGB(230, 58, 47, 8));
            }

            if (m_Icon)
            {
                m_Icon.SetImage(3);
            }

            if (m_Status)
            {
                m_Status.SetText("TRADER");
                m_Status.SetColor(ARGB(255, 244, 211, 94));
            }

            if (m_Zone)
            {
                m_Zone.SetText(zoneName == "" ? "TRADER ZONE" : zoneName);
            }
        }
        else if (zoneType == "admin")
        {
            if (m_Panel)
            {
                m_Panel.SetColor(ARGB(230, 61, 17, 58));
            }

            if (m_Icon)
            {
                m_Icon.SetImage(4);
            }

            if (m_Status)
            {
                m_Status.SetText("ADMIN");
                m_Status.SetColor(ARGB(255, 242, 140, 255));
            }

            if (m_Zone)
            {
                m_Zone.SetText(zoneName == "" ? "ADMIN ZONE" : zoneName);
            }
        }
        else if (zoneType == "event")
        {
            if (m_Panel)
            {
                m_Panel.SetColor(ARGB(230, 31, 24, 51));
            }

            if (m_Icon)
            {
                m_Icon.SetImage(5);
            }

            if (m_Status)
            {
                m_Status.SetText("EVENT");
                m_Status.SetColor(ARGB(255, 156, 123, 255));
            }

            if (m_Zone)
            {
                m_Zone.SetText(zoneName == "" ? "EVENT ZONE" : zoneName);
            }
        }
        else if (zoneType == "radiation")
        {
            if (m_Panel)
            {
                m_Panel.SetColor(ARGB(230, 28, 45, 16));
            }

            if (m_Icon)
            {
                m_Icon.SetImage(6);
            }

            if (m_Status)
            {
                m_Status.SetText("RADIATION");
                m_Status.SetColor(ARGB(255, 139, 224, 78));
            }

            if (m_Zone)
            {
                m_Zone.SetText(zoneName == "" ? "RADIATION ZONE" : zoneName);
            }
        }
        else if (zoneType == "raid")
        {
            if (m_Panel)
            {
                m_Panel.SetColor(ARGB(230, 51, 36, 19));
            }

            if (m_Icon)
            {
                m_Icon.SetImage(7);
            }

            if (m_Status)
            {
                m_Status.SetText("RAID");
                m_Status.SetColor(ARGB(255, 255, 180, 93));
            }

            if (m_Zone)
            {
                m_Zone.SetText(zoneName == "" ? "RAID ZONE" : zoneName);
            }
        }
        else
        {
            if (m_Panel)
            {
                m_Panel.SetColor(ARGB(230, 6, 46, 26));
            }

            if (m_Icon)
            {
                m_Icon.SetImage(0);
            }

            if (m_Status)
            {
                m_Status.SetText("SAFE");
                m_Status.SetColor(ARGB(255, 87, 242, 135));
            }

            if (m_Zone)
            {
                m_Zone.SetText("SAFE ZONE");
            }
        }
    }
}

modded class MissionGameplay
{
    protected ref SentinelUIZoneStatusHUD m_SentinelUIZoneStatusHUD;
    protected ref SentinelUIPurgeOverlay m_SentinelUIPurgeOverlay;
    protected int m_SentinelUIZoneSequence = -1;
    protected int m_SentinelUIPurgeSequence = -1;

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        if (!GetGame() || !GetGame().IsClient())
        {
            return;
        }

        Man player = GetGame().GetPlayer();

        if (!player || !player.GetIdentity())
        {
            return;
        }

        if (!m_SentinelUIZoneStatusHUD)
        {
            m_SentinelUIZoneStatusHUD = new SentinelUIZoneStatusHUD();

            if (!m_SentinelUIZoneStatusHUD.Init())
            {
                m_SentinelUIZoneStatusHUD = null;
                return;
            }

            m_SentinelUIZoneSequence = SentinelUIZoneState.GetSequence();
            m_SentinelUIZoneStatusHUD.SetStatus(SentinelUIZoneState.GetZoneType(), SentinelUIZoneState.GetZoneName(), SentinelUIPurgeState.IsActive());

            m_SentinelUIPurgeOverlay = new SentinelUIPurgeOverlay();

            if (!m_SentinelUIPurgeOverlay.Init())
            {
                m_SentinelUIPurgeOverlay = null;
            }

            SentinelUIMapZoneStore.RequestFromServer();
            SentinelUIPurgeState.RequestFromServer();
            Print("[SentinelUI] zone status HUD ready");
        }

        int sequence = SentinelUIZoneState.GetSequence();

        if (sequence != m_SentinelUIZoneSequence)
        {
            m_SentinelUIZoneSequence = sequence;
            m_SentinelUIZoneStatusHUD.SetStatus(SentinelUIZoneState.GetZoneType(), SentinelUIZoneState.GetZoneName(), SentinelUIPurgeState.IsActive());
        }

        int purgeSequence = SentinelUIPurgeState.GetSequence();

        if (purgeSequence != m_SentinelUIPurgeSequence)
        {
            m_SentinelUIPurgeSequence = purgeSequence;
            m_SentinelUIZoneStatusHUD.SetStatus(SentinelUIZoneState.GetZoneType(), SentinelUIZoneState.GetZoneName(), SentinelUIPurgeState.IsActive());

            if (m_SentinelUIPurgeOverlay)
            {
                m_SentinelUIPurgeOverlay.ApplyState(SentinelUIPurgeState.GetLastKind(), SentinelUIPurgeState.GetCountdownSeconds(), SentinelUIPurgeState.GetEventName());
            }
        }

        if (m_SentinelUIPurgeOverlay)
        {
            m_SentinelUIPurgeOverlay.Update(timeslice);
        }
    }

    override void OnMissionFinish()
    {
        if (m_SentinelUIZoneStatusHUD)
        {
            m_SentinelUIZoneStatusHUD.Destroy();
            m_SentinelUIZoneStatusHUD = null;
        }

        if (m_SentinelUIPurgeOverlay)
        {
            m_SentinelUIPurgeOverlay.Destroy();
            m_SentinelUIPurgeOverlay = null;
        }

        m_SentinelUIZoneSequence = -1;
        m_SentinelUIPurgeSequence = -1;
        SentinelUIMapZoneStore.Reset();
        SentinelUIPurgeState.Reset();
        super.OnMissionFinish();
    }
}
