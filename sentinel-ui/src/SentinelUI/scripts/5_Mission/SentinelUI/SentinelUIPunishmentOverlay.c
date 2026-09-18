class SentinelUIPunishmentOverlay
{
    protected Widget m_Root;
    protected TextWidget m_Title;
    protected TextWidget m_Message;
    protected float m_DisplaySeconds;

    bool Init()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("SentinelUI/gui/layouts/sentinel_punishment_warning.layout");

        if (!m_Root)
        {
            Print("[SentinelUI] failed to create punishment warning layout");
            return false;
        }

        m_Title = TextWidget.Cast(m_Root.FindAnyWidget("SentinelUIPunishmentTitle"));
        m_Message = TextWidget.Cast(m_Root.FindAnyWidget("SentinelUIPunishmentMessage"));
        m_Root.Show(false);
        return true;
    }

    void ShowWarning(int count, string message)
    {
        if (!m_Root)
        {
            return;
        }

        if (m_Title)
        {
            m_Title.SetText(GetOrdinal(count) + " WARNING");
        }

        if (m_Message)
        {
            m_Message.SetText(message);
        }

        m_DisplaySeconds = 6.0;
        m_Root.Show(true);
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
            m_Root.Show(false);
        }
    }

    void Destroy()
    {
        if (m_Root)
        {
            m_Root.Unlink();
        }

        m_Root = null;
        m_Title = null;
        m_Message = null;
    }

    protected string GetOrdinal(int value)
    {
        int lastTwo = value % 100;
        if (lastTwo >= 11 && lastTwo <= 13)
        {
            return value.ToString() + "TH";
        }

        int last = value % 10;
        if (last == 1) return value.ToString() + "ST";
        if (last == 2) return value.ToString() + "ND";
        if (last == 3) return value.ToString() + "RD";
        return value.ToString() + "TH";
    }
}
