class SentinelUIPurgeOverlay
{
    protected Widget m_Root;
    protected Widget m_Panel;
    protected ImageWidget m_Icon;
    protected TextWidget m_Title;
    protected TextWidget m_Value;
    protected TextWidget m_Subtitle;
    protected EffectSound m_SirenEffect;
    protected float m_DisplaySeconds;

    bool Init()
    {
        if (!GetGame() || !GetGame().GetWorkspace())
        {
            return false;
        }

        m_Root = GetGame().GetWorkspace().CreateWidgets("SentinelUI/gui/layouts/sentinel_purge_event.layout");

        if (!m_Root)
        {
            Print("[SentinelUI] failed to create Purge event layout");
            return false;
        }

        m_Panel = m_Root.FindAnyWidget("SentinelUIPurgePanel");
        m_Icon = ImageWidget.Cast(m_Root.FindAnyWidget("SentinelUIPurgeIcon"));
        m_Title = TextWidget.Cast(m_Root.FindAnyWidget("SentinelUIPurgeTitle"));
        m_Value = TextWidget.Cast(m_Root.FindAnyWidget("SentinelUIPurgeValue"));
        m_Subtitle = TextWidget.Cast(m_Root.FindAnyWidget("SentinelUIPurgeSubtitle"));

        if (m_Icon)
        {
            m_Icon.LoadImageFile(0, "SentinelUI/gui/icons/purge_ca.paa");
            m_Icon.SetImage(0);
        }

        m_Root.Show(false);
        return true;
    }

    void Destroy()
    {
        StopSiren();

        if (m_Root)
        {
            m_Root.Unlink();
        }

        m_Root = null;
        m_Panel = null;
        m_Icon = null;
        m_Title = null;
        m_Value = null;
        m_Subtitle = null;
    }

    void ApplyState(int kind, int countdownSeconds, string eventName)
    {
        if (!m_Root)
        {
            return;
        }

        if (eventName == "")
        {
            eventName = "Purge";
        }

        if (kind == SentinelUIZoneProtocol.KIND_PURGE_COUNTDOWN)
        {
            SetPanelColor(ARGB(242, 57, 4, 7));
            SetText("PURGE BEGINS IN", countdownSeconds.ToString(), eventName);
            m_DisplaySeconds = 2.0;
            m_Root.Show(true);
        }
        else if (kind == SentinelUIZoneProtocol.KIND_PURGE_START)
        {
            SetPanelColor(ARGB(247, 74, 2, 5));
            SetText("PURGE ACTIVE", "!", "MAP-WIDE PVP");
            m_DisplaySeconds = 8.0;
            m_Root.Show(true);
            PlaySiren();
        }
        else if (kind == SentinelUIZoneProtocol.KIND_PURGE_END)
        {
            SetPanelColor(ARGB(242, 24, 34, 28));
            SetText("PURGE ENDED", "", "NORMAL RULES RESTORED");
            m_DisplaySeconds = 5.0;
            m_Root.Show(true);
            StopSiren();
        }
        else if (kind == SentinelUIZoneProtocol.KIND_PURGE_SYNC)
        {
            m_Root.Show(false);
        }
    }

    void Update(float timeslice)
    {
        if (!m_Root || m_DisplaySeconds <= 0)
        {
            return;
        }

        m_DisplaySeconds -= timeslice;

        if (m_DisplaySeconds <= 0)
        {
            m_DisplaySeconds = 0;
            m_Root.Show(false);
        }
    }

    protected void SetPanelColor(int color)
    {
        if (m_Panel)
        {
            m_Panel.SetColor(color);
        }
    }

    protected void SetText(string title, string value, string subtitle)
    {
        if (m_Title)
        {
            m_Title.SetText(title);
        }

        if (m_Value)
        {
            m_Value.SetText(value);
        }

        if (m_Subtitle)
        {
            m_Subtitle.SetText(subtitle);
        }
    }

    protected void PlaySiren()
    {
        StopSiren();

        if (!GetGame().ConfigIsExisting("CfgSoundSets SentinelUI_Purge_SoundSet"))
        {
            Print("[SentinelUI] Purge siren failed: CfgSoundSets SentinelUI_Purge_SoundSet is missing");
            return;
        }

        if (!GetGame().ConfigIsExisting("CfgSoundShaders SentinelUI_Purge_SoundShader"))
        {
            Print("[SentinelUI] Purge siren failed: CfgSoundShaders SentinelUI_Purge_SoundShader is missing");
            return;
        }

        Print("[SentinelUI] Purge sound configuration found; requesting PCM playback");

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        if (!player)
        {
            Print("[SentinelUI] Purge siren failed: local player is unavailable");
            return;
        }

        m_SirenEffect = SEffectManager.PlaySoundOnObject("SentinelUI_Purge_SoundSet", player, 0.1, 0.5, false);

        if (m_SirenEffect)
        {
            m_SirenEffect.SetSoundAutodestroy(true);
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(CheckSirenPlayback, 750, false);
            Print("[SentinelUI] Purge siren requested on local player effect channel");
        }
        else
        {
            Print("[SentinelUI] Purge siren failed: effect could not be created");
        }
    }

    protected void CheckSirenPlayback()
    {
        if (m_SirenEffect && m_SirenEffect.IsSoundPlaying())
        {
            Print("[SentinelUI] Purge siren playback confirmed");
        }
        else
        {
            Print("[SentinelUI] Purge siren playback failed after request");
        }
    }

    protected void StopSiren()
    {
        if (m_SirenEffect)
        {
            m_SirenEffect.SoundStop();
            m_SirenEffect = null;
        }
    }
}
