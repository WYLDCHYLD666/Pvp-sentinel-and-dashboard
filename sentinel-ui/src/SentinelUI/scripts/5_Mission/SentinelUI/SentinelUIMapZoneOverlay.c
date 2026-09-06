class SentinelUIMapZoneLabel
{
    Widget Root;
    TextWidget Text;
}

class SentinelUIMapZoneOverlay
{
    static const int CIRCLE_SEGMENTS = 48;
    static const int MAX_HATCH_SEGMENTS_PER_ZONE = 192;
    static const float GLOW_WIDTH = 7.0;
    static const float LINE_WIDTH = 3.0;
    static const float HATCH_WIDTH = 2.0;
    static const float HATCH_SPACING = 28.0;
    static const float INVERSE_SQUARE_ROOT_TWO = 0.70710678;

    protected MapWidget m_Map;
    protected Widget m_Root;
    protected ref array<ref SentinelUIMapZoneLabel> m_Labels = new array<ref SentinelUIMapZoneLabel>();
    protected ref array<ImageWidget> m_Segments = new array<ImageWidget>();
    protected int m_LastSequence = -1;
    protected int m_SegmentCursor;
    protected int m_BoundarySegmentCount;
    protected bool m_HatchReadyLogged;
    protected float m_RootX;
    protected float m_RootY;
    protected float m_RootWidth;
    protected float m_RootHeight;

    bool Init(MapWidget mapWidget, Widget overlayParent)
    {
        if (!mapWidget || !overlayParent || !GetGame() || !GetGame().GetWorkspace())
        {
            return false;
        }

        m_Map = mapWidget;
        m_Root = GetGame().GetWorkspace().CreateWidgets("SentinelUI/gui/layouts/sentinel_map_zone_overlay.layout", overlayParent);

        if (!m_Root)
        {
            Print("[SentinelUI] failed to create map-zone overlay");
            return false;
        }

        m_Root.Show(true);
        return true;
    }

    void Destroy()
    {
        ClearVisuals();

        if (m_Root)
        {
            m_Root.Unlink();
        }

        m_Root = null;
        m_Map = null;
    }

    void Update()
    {
        if (!m_Map || !m_Root)
        {
            return;
        }

        int sequence = SentinelUIMapZoneStore.GetSequence();

        if (sequence != m_LastSequence)
        {
            m_LastSequence = sequence;
            RebuildVisuals();
        }

        m_Root.GetScreenPos(m_RootX, m_RootY);
        m_Root.GetScreenSize(m_RootWidth, m_RootHeight);
        m_SegmentCursor = 0;

        array<ref SentinelUIMapZoneData> zones = SentinelUIMapZoneStore.GetZones();

        if (zones)
        {
            for (int i = 0; i < zones.Count(); i++)
            {
                SentinelUIMapZoneLabel label;

                if (i < m_Labels.Count())
                {
                    label = m_Labels[i];
                }

                RenderZone(zones[i], label);
            }
        }

        HideRemainingSegments();

        if (!m_HatchReadyLogged && m_SegmentCursor > m_BoundarySegmentCount)
        {
            m_HatchReadyLogged = true;
            Print("[SentinelUI] map zone hatching ready active-segments=" + (m_SegmentCursor - m_BoundarySegmentCount).ToString());
        }
    }

    protected void RebuildVisuals()
    {
        ClearVisuals();

        array<ref SentinelUIMapZoneData> zones = SentinelUIMapZoneStore.GetZones();

        if (!zones)
        {
            return;
        }

        int requestedSegments;

        foreach (SentinelUIMapZoneData zone : zones)
        {
            Widget labelRoot = GetGame().GetWorkspace().CreateWidgets("SentinelUI/gui/layouts/sentinel_map_zone_label.layout", m_Root);
            ref SentinelUIMapZoneLabel label = new SentinelUIMapZoneLabel();

            if (labelRoot)
            {
                label.Root = labelRoot;
                label.Text = TextWidget.Cast(labelRoot.FindAnyWidget("SentinelUIMapZoneLabelText"));
                label.Root.SetColor(GetPanelColor(zone));

                if (label.Text)
                {
                    label.Text.SetText(GetLabelText(zone));
                    label.Text.SetColor(GetLineColor(zone));
                }

                label.Root.Show(false);
            }

            m_Labels.Insert(label);

            int segmentCount = GetEdgeCount(zone) * 2;
            requestedSegments += segmentCount;

            for (int segmentIndex = 0; segmentIndex < segmentCount; segmentIndex++)
            {
                CreateSegment();
            }
        }

        m_BoundarySegmentCount = requestedSegments;
        m_HatchReadyLogged = false;
        Print("[SentinelUI] map widget renderer ready zones=" + zones.Count().ToString() + " segments=" + m_Segments.Count().ToString() + "/" + requestedSegments.ToString());
    }

    protected int GetEdgeCount(SentinelUIMapZoneData zone)
    {
        if (!zone || !zone.Coordinates)
        {
            return 0;
        }

        if (zone.GeometryKind == "circle" && zone.Coordinates.Count() == 2 && zone.Radius > 0)
        {
            return CIRCLE_SEGMENTS;
        }

        if (zone.GeometryKind == "polygon" && zone.Coordinates.Count() >= 6)
        {
            return zone.Coordinates.Count() / 2;
        }

        return 0;
    }

    protected void ClearVisuals()
    {
        foreach (SentinelUIMapZoneLabel label : m_Labels)
        {
            if (label && label.Root)
            {
                label.Root.Unlink();
            }
        }

        m_Labels.Clear();

        foreach (ImageWidget segment : m_Segments)
        {
            if (segment)
            {
                segment.Unlink();
            }
        }

        m_Segments.Clear();
        m_SegmentCursor = 0;
        m_BoundarySegmentCount = 0;
        m_HatchReadyLogged = false;
    }

    protected void HideRemainingSegments()
    {
        for (int i = m_SegmentCursor; i < m_Segments.Count(); i++)
        {
            if (m_Segments[i])
            {
                m_Segments[i].Show(false);
            }
        }
    }

    protected ImageWidget TakeNextSegment()
    {
        if (m_SegmentCursor >= m_Segments.Count())
        {
            if (!CreateSegment())
            {
                return null;
            }
        }

        ImageWidget segment = m_Segments[m_SegmentCursor];
        m_SegmentCursor++;
        return segment;
    }

    protected ImageWidget CreateSegment()
    {
        Widget segmentRoot = GetGame().GetWorkspace().CreateWidgets("SentinelUI/gui/layouts/sentinel_map_zone_segment.layout", m_Root);
        ImageWidget segment = ImageWidget.Cast(segmentRoot);

        if (segment && segment.LoadImageFile(0, "SentinelUI/gui/textures/white_ca.paa"))
        {
            segment.SetImage(0);
            segment.SetSort(300);
            segment.Show(false);
            m_Segments.Insert(segment);
            return segment;
        }

        if (segmentRoot)
        {
            segmentRoot.Unlink();
        }

        return null;
    }

    protected void RenderZone(SentinelUIMapZoneData zone, SentinelUIMapZoneLabel label)
    {
        if (!zone || !zone.Coordinates)
        {
            return;
        }

        int glowColor = GetGlowColor(zone);
        int lineColor = GetLineColor(zone);
        RenderHatching(zone, GetHatchColor(zone));

        if (zone.GeometryKind == "circle")
        {
            RenderCircle(zone, glowColor, lineColor);
        }
        else if (zone.GeometryKind == "polygon")
        {
            RenderPolygon(zone, glowColor, lineColor);
        }

        PositionLabel(zone, label);
    }

    protected void RenderHatching(SentinelUIMapZoneData zone, int hatchColor)
    {
        ref array<vector> screenPoints = BuildScreenPolygon(zone);

        if (!screenPoints || screenPoints.Count() < 3)
        {
            return;
        }

        float minimumV = 1000000.0;
        float maximumV = -1000000.0;

        foreach (vector point : screenPoints)
        {
            float pointV = (-point[0] + point[1]) * INVERSE_SQUARE_ROOT_TWO;

            if (pointV < minimumV)
            {
                minimumV = pointV;
            }

            if (pointV > maximumV)
            {
                maximumV = pointV;
            }
        }

        float viewportMinimumV = -m_RootWidth * INVERSE_SQUARE_ROOT_TWO;
        float viewportMaximumV = m_RootHeight * INVERSE_SQUARE_ROOT_TWO;

        if (minimumV < viewportMinimumV)
        {
            minimumV = viewportMinimumV;
        }

        if (maximumV > viewportMaximumV)
        {
            maximumV = viewportMaximumV;
        }

        int hatchSegmentCount;
        ref array<float> intersections = new array<float>();

        for (float scanV = minimumV + (HATCH_SPACING * 0.5); scanV < maximumV; scanV += HATCH_SPACING)
        {
            intersections.Clear();

            for (int edgeIndex = 0; edgeIndex < screenPoints.Count(); edgeIndex++)
            {
                vector pointA = screenPoints[edgeIndex];
                vector pointB = screenPoints[(edgeIndex + 1) % screenPoints.Count()];
                float uA = (pointA[0] + pointA[1]) * INVERSE_SQUARE_ROOT_TWO;
                float vA = (-pointA[0] + pointA[1]) * INVERSE_SQUARE_ROOT_TWO;
                float uB = (pointB[0] + pointB[1]) * INVERSE_SQUARE_ROOT_TWO;
                float vB = (-pointB[0] + pointB[1]) * INVERSE_SQUARE_ROOT_TWO;

                if ((vA <= scanV && vB > scanV) || (vB <= scanV && vA > scanV))
                {
                    float interpolation = (scanV - vA) / (vB - vA);
                    intersections.Insert(uA + ((uB - uA) * interpolation));
                }
            }

            intersections.Sort();

            for (int intersectionIndex = 0; intersectionIndex + 1 < intersections.Count(); intersectionIndex += 2)
            {
                DrawHatchSegment(intersections[intersectionIndex], intersections[intersectionIndex + 1], scanV, hatchColor);
                hatchSegmentCount++;

                if (hatchSegmentCount >= MAX_HATCH_SEGMENTS_PER_ZONE)
                {
                    return;
                }
            }
        }
    }

    protected array<vector> BuildScreenPolygon(SentinelUIMapZoneData zone)
    {
        ref array<vector> points = new array<vector>();

        if (zone.GeometryKind == "circle" && zone.Coordinates.Count() == 2 && zone.Radius > 0)
        {
            float centreX = zone.Coordinates[0];
            float centreZ = zone.Coordinates[1];

            for (int circleIndex = 0; circleIndex < CIRCLE_SEGMENTS; circleIndex++)
            {
                float angle = Math.PI2 * circleIndex / CIRCLE_SEGMENTS;
                vector circleWorldPoint = Vector(centreX + Math.Cos(angle) * zone.Radius, 0, centreZ + Math.Sin(angle) * zone.Radius);
                vector circleScreenPoint = m_Map.MapToScreen(circleWorldPoint);
                points.Insert(Vector(circleScreenPoint[0] - m_RootX, circleScreenPoint[1] - m_RootY, 0));
            }
        }
        else if (zone.GeometryKind == "polygon" && zone.Coordinates.Count() >= 6)
        {
            int pointCount = zone.Coordinates.Count() / 2;

            for (int polygonIndex = 0; polygonIndex < pointCount; polygonIndex++)
            {
                vector polygonWorldPoint = Vector(zone.Coordinates[polygonIndex * 2], 0, zone.Coordinates[(polygonIndex * 2) + 1]);
                vector polygonScreenPoint = m_Map.MapToScreen(polygonWorldPoint);
                points.Insert(Vector(polygonScreenPoint[0] - m_RootX, polygonScreenPoint[1] - m_RootY, 0));
            }
        }

        return points;
    }

    protected void DrawHatchSegment(float startU, float endU, float scanV, int hatchColor)
    {
        float startX = (startU - scanV) * INVERSE_SQUARE_ROOT_TWO;
        float startY = (startU + scanV) * INVERSE_SQUARE_ROOT_TWO;
        float endX = (endU - scanV) * INVERSE_SQUARE_ROOT_TWO;
        float endY = (endU + scanV) * INVERSE_SQUARE_ROOT_TWO;

        if ((startX < 0 && endX < 0) || (startX > m_RootWidth && endX > m_RootWidth) || (startY < 0 && endY < 0) || (startY > m_RootHeight && endY > m_RootHeight))
        {
            return;
        }

        float deltaX = endX - startX;
        float deltaY = endY - startY;
        float length = Math.Sqrt((deltaX * deltaX) + (deltaY * deltaY));

        if (length < 1.0)
        {
            return;
        }

        ImageWidget hatchSegment = TakeNextSegment();

        if (!hatchSegment)
        {
            return;
        }

        float middleX = (startX + endX) * 0.5;
        float middleY = (startY + endY) * 0.5;
        float rotation = Math.Atan2(deltaY, deltaX) * Math.RAD2DEG;
        hatchSegment.SetSort(285);
        PositionSegment(hatchSegment, middleX, middleY, length, HATCH_WIDTH, rotation, hatchColor);
    }

    protected void RenderCircle(SentinelUIMapZoneData zone, int glowColor, int lineColor)
    {
        if (zone.Coordinates.Count() != 2 || zone.Radius <= 0)
        {
            return;
        }

        float centreX = zone.Coordinates[0];
        float centreZ = zone.Coordinates[1];
        vector previousPoint = "0 0 0";

        for (int i = 0; i <= CIRCLE_SEGMENTS; i++)
        {
            float angle = Math.PI2 * i / CIRCLE_SEGMENTS;
            vector currentPoint = Vector(centreX + Math.Cos(angle) * zone.Radius, 0, centreZ + Math.Sin(angle) * zone.Radius);

            if (i > 0)
            {
                DrawSegment(previousPoint, currentPoint, glowColor, lineColor);
            }

            previousPoint = currentPoint;
        }
    }

    protected void RenderPolygon(SentinelUIMapZoneData zone, int glowColor, int lineColor)
    {
        int pointCount = zone.Coordinates.Count() / 2;

        if (pointCount < 3)
        {
            return;
        }

        for (int i = 0; i < pointCount; i++)
        {
            int nextIndex = (i + 1) % pointCount;
            vector currentPoint = Vector(zone.Coordinates[i * 2], 0, zone.Coordinates[(i * 2) + 1]);
            vector nextPoint = Vector(zone.Coordinates[nextIndex * 2], 0, zone.Coordinates[(nextIndex * 2) + 1]);
            DrawSegment(currentPoint, nextPoint, glowColor, lineColor);
        }
    }

    protected void DrawSegment(vector worldStart, vector worldEnd, int glowColor, int lineColor)
    {
        ImageWidget glowSegment = TakeNextSegment();
        ImageWidget lineSegment = TakeNextSegment();

        if (!glowSegment || !lineSegment)
        {
            if (glowSegment)
            {
                glowSegment.Show(false);
            }

            if (lineSegment)
            {
                lineSegment.Show(false);
            }

            return;
        }

        vector screenStart = m_Map.MapToScreen(worldStart);
        vector screenEnd = m_Map.MapToScreen(worldEnd);
        float startX = screenStart[0] - m_RootX;
        float startY = screenStart[1] - m_RootY;
        float endX = screenEnd[0] - m_RootX;
        float endY = screenEnd[1] - m_RootY;

        if ((startX < 0 && endX < 0) || (startX > m_RootWidth && endX > m_RootWidth) || (startY < 0 && endY < 0) || (startY > m_RootHeight && endY > m_RootHeight))
        {
            glowSegment.Show(false);
            lineSegment.Show(false);
            return;
        }

        float deltaX = endX - startX;
        float deltaY = endY - startY;
        float length = Math.Sqrt((deltaX * deltaX) + (deltaY * deltaY));

        if (length < 1.0)
        {
            glowSegment.Show(false);
            lineSegment.Show(false);
            return;
        }

        float middleX = (startX + endX) * 0.5;
        float middleY = (startY + endY) * 0.5;
        float rotation = Math.Atan2(deltaY, deltaX) * Math.RAD2DEG;
        glowSegment.SetSort(300);
        lineSegment.SetSort(300);
        PositionSegment(glowSegment, middleX, middleY, length, GLOW_WIDTH, rotation, glowColor);
        PositionSegment(lineSegment, middleX, middleY, length, LINE_WIDTH, rotation, lineColor);
    }

    protected void PositionSegment(ImageWidget segment, float middleX, float middleY, float length, float width, float rotation, int color)
    {
        segment.SetSize(length, width);
        segment.SetPos(middleX - (length * 0.5), middleY - (width * 0.5));
        segment.SetRotation(0, 0, rotation);
        segment.SetColor(color);
        segment.Show(true);
    }

    protected void PositionLabel(SentinelUIMapZoneData zone, SentinelUIMapZoneLabel label)
    {
        if (!label || !label.Root)
        {
            return;
        }

        vector worldPosition = GetLabelWorldPosition(zone);
        vector screenPosition = m_Map.MapToScreen(worldPosition);
        float localX = screenPosition[0] - m_RootX;
        float localY = screenPosition[1] - m_RootY;

        if (localX < 0 || localX > m_RootWidth || localY < 0 || localY > m_RootHeight)
        {
            label.Root.Show(false);
            return;
        }

        float labelWidth;
        float labelHeight;
        label.Root.GetSize(labelWidth, labelHeight);
        label.Root.SetPos(localX - (labelWidth * 0.5), localY - (labelHeight * 0.5));
        label.Root.Show(true);
    }

    protected vector GetLabelWorldPosition(SentinelUIMapZoneData zone)
    {
        if (zone.GeometryKind == "circle" && zone.Coordinates.Count() == 2)
        {
            return Vector(zone.Coordinates[0], 0, zone.Coordinates[1]);
        }

        float totalX;
        float totalZ;
        int pointCount = zone.Coordinates.Count() / 2;

        for (int i = 0; i < pointCount; i++)
        {
            totalX += zone.Coordinates[i * 2];
            totalZ += zone.Coordinates[(i * 2) + 1];
        }

        if (pointCount == 0)
        {
            return "0 0 0";
        }

        return Vector(totalX / pointCount, 0, totalZ / pointCount);
    }

    protected string GetLabelText(SentinelUIMapZoneData zone)
    {
        if (zone.Type == "pvp")
        {
            return "PVP | " + zone.Name;
        }

        if (zone.Type == "safe")
        {
            return "SAFE | " + zone.Name;
        }

        return "ZONE | " + zone.Name;
    }

    protected int GetLineColor(SentinelUIMapZoneData zone)
    {
        int red;
        int green;
        int blue;
        GetZoneRgb(zone, red, green, blue);
        return ARGB(255, red, green, blue);
    }

    protected int GetGlowColor(SentinelUIMapZoneData zone)
    {
        int red;
        int green;
        int blue;
        GetZoneRgb(zone, red, green, blue);
        return ARGB(95, red, green, blue);
    }

    protected int GetHatchColor(SentinelUIMapZoneData zone)
    {
        int red;
        int green;
        int blue;
        GetZoneRgb(zone, red, green, blue);
        return ARGB(52, red, green, blue);
    }

    protected int GetPanelColor(SentinelUIMapZoneData zone)
    {
        int red;
        int green;
        int blue;
        GetZoneRgb(zone, red, green, blue);
        return ARGB(220, red / 5, green / 5, blue / 5);
    }

    protected void GetZoneRgb(SentinelUIMapZoneData zone, out int red, out int green, out int blue)
    {
        if (zone.Type == "pvp")
        {
            red = 255;
            green = 97;
            blue = 124;
            return;
        }

        if (zone.Type == "safe")
        {
            red = 68;
            green = 215;
            blue = 161;
            return;
        }

        if (zone.Type == "trader")
        {
            red = 244;
            green = 211;
            blue = 94;
            return;
        }

        if (zone.Type == "event")
        {
            red = 156;
            green = 123;
            blue = 255;
            return;
        }

        if (zone.Type == "radiation")
        {
            red = 139;
            green = 224;
            blue = 78;
            return;
        }

        if (zone.Type == "admin")
        {
            red = 242;
            green = 140;
            blue = 255;
            return;
        }

        if (zone.Type == "raid")
        {
            red = 255;
            green = 180;
            blue = 93;
            return;
        }

        red = 53;
        green = 200;
        blue = 245;
    }
}

modded class MapMenu
{
    protected ref SentinelUIMapZoneOverlay m_SentinelUIMapZoneOverlay;

    override Widget Init()
    {
        Widget root = super.Init();

        if (m_SentinelUIMapZoneOverlay)
        {
            m_SentinelUIMapZoneOverlay.Destroy();
            m_SentinelUIMapZoneOverlay = null;
        }

        if (m_MapWidgetInstance)
        {
            m_SentinelUIMapZoneOverlay = new SentinelUIMapZoneOverlay();

            if (!m_SentinelUIMapZoneOverlay.Init(m_MapWidgetInstance, root))
            {
                m_SentinelUIMapZoneOverlay = null;
            }
        }

        SentinelUIMapZoneStore.RequestFromServer();
        return root;
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);

        if (m_SentinelUIMapZoneOverlay)
        {
            m_SentinelUIMapZoneOverlay.Update();
        }
    }
}
