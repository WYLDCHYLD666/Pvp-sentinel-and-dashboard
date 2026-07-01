class PvPZoneManager
{
    protected static ref PvPZoneManager s_Instance;
    protected ref array<ref PvPZone> m_Zones;
    protected bool m_Initialized;

    static PvPZoneManager GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new PvPZoneManager();
        }

        return s_Instance;
    }

    void PvPZoneManager()
    {
        m_Zones = new array<ref PvPZone>();
        m_Initialized = false;
    }

    void Init()
    {
        if (!m_Zones)
        {
            m_Zones = new array<ref PvPZone>();
        }

        m_Zones.Clear();

        m_Zones.Insert(new PvPZone("NWAF", "4750 0 10200", 1200.0));
        m_Zones.Insert(new PvPZone("Tisy", "1650 0 14250", 900.0));

        m_Initialized = true;

        Print(PvPUltimateConstants.LOG_PREFIX + " zones initialized");
    }

    bool InZone(vector position)
    {
        if (!m_Initialized)
        {
            Init();
        }

        foreach (PvPZone zone : m_Zones)
        {
            if (zone && zone.Contains(position))
            {
                return true;
            }
        }

        return false;
    }

    bool IsWeekendPvP()
    {
        int year;
        int month;
        int day;

        GetYearMonthDayUTC(year, month, day);

        int weekday = GetWeekDay(year, month, day);

        return weekday == 0 || weekday == 6;
    }
}
