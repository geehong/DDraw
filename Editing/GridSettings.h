#ifndef GRID_SETTINGS_H
#define GRID_SETTINGS_H

enum class GridUnit
{
    Point,  // 1 pt = 1.0 px (web/screen unit)
    MM,     // 1 mm = ~3.7795 px (assuming 96 DPI)
    CM,     // 1 cm = ~37.795 px
    M,      // 1 m = ~3779.5 px
    Inch    // 1 in = 96.0 px
};

inline const wchar_t* GetGridUnitName(GridUnit unit)
{
    switch (unit)
    {
    case GridUnit::Point: return L"Point (pt)";
    case GridUnit::MM:    return L"Millimeter (mm)";
    case GridUnit::CM:    return L"Centimeter (cm)";
    case GridUnit::M:     return L"Meter (m)";
    case GridUnit::Inch:  return L"Inch (in)";
    default:              return L"Point (pt)";
    }
}

inline float GetPixelsPerUnit(GridUnit unit)
{
    switch (unit)
    {
    case GridUnit::Point: return 1.0f;
    case GridUnit::MM:    return 3.779527559f; // 96 / 25.4
    case GridUnit::CM:    return 37.79527559f;
    case GridUnit::M:     return 3779.527559f;
    case GridUnit::Inch:  return 96.0f;
    default:              return 1.0f;
    }
}

class GridSettings
{
private:
    GridUnit m_unit;
    bool     m_bShowGrid;
    float    m_subGridStepUnit;   // Small grid step in unit (e.g. 5 pt or 1 mm)
    float    m_mainGridStepUnit;  // Major grid step in unit (e.g. 10 pt or 10 mm)
    float    m_subGridLineWidth;  // e.g. 0.1 px
    float    m_mainGridLineWidth; // e.g. 0.5 px
    bool     m_bSnapToGrid;
    float    m_snapIntervalUnit; // Grid snap interval in unit (e.g. 5 pt, 1 mm)

public:
    GridSettings()
        : m_unit(GridUnit::Point),
          m_bShowGrid(true),
          m_subGridStepUnit(5.0f),
          m_mainGridStepUnit(10.0f),
          m_subGridLineWidth(0.1f),
          m_mainGridLineWidth(0.5f),
          m_bSnapToGrid(true),
          m_snapIntervalUnit(5.0f) {}

    GridUnit GetUnit() const { return m_unit; }
    void SetUnit(GridUnit unit)
    {
        m_unit = unit;
        // Apply sensible defaults per unit
        switch (unit)
        {
        case GridUnit::Point:
            m_subGridStepUnit = 5.0f;
            m_mainGridStepUnit = 25.0f;
            m_snapIntervalUnit = 5.0f;
            break;
        case GridUnit::MM:
            m_subGridStepUnit = 1.0f;
            m_mainGridStepUnit = 10.0f;
            m_snapIntervalUnit = 1.0f;
            break;
        case GridUnit::CM:
            m_subGridStepUnit = 0.5f;
            m_mainGridStepUnit = 1.0f;
            m_snapIntervalUnit = 0.5f;
            break;
        case GridUnit::M:
            m_subGridStepUnit = 0.05f;
            m_mainGridStepUnit = 0.1f;
            m_snapIntervalUnit = 0.05f;
            break;
        case GridUnit::Inch:
            m_subGridStepUnit = 0.125f;
            m_mainGridStepUnit = 0.5f;
            m_snapIntervalUnit = 0.125f;
            break;
        }
    }

    bool IsGridVisible() const { return m_bShowGrid; }
    void SetGridVisible(bool visible) { m_bShowGrid = visible; }

    float GetSubGridStepUnit() const { return m_subGridStepUnit; }
    void SetSubGridStepUnit(float step) { if (step > 0.0f) m_subGridStepUnit = step; }

    float GetMainGridStepUnit() const { return m_mainGridStepUnit; }
    void SetMainGridStepUnit(float step) { if (step > 0.0f) m_mainGridStepUnit = step; }

    float GetSubGridLineWidth() const { return m_subGridLineWidth; }
    void SetSubGridLineWidth(float width) { if (width > 0.0f) m_subGridLineWidth = width; }

    float GetMainGridLineWidth() const { return m_mainGridLineWidth; }
    void SetMainGridLineWidth(float width) { if (width > 0.0f) m_mainGridLineWidth = width; }

    bool IsSnapToGrid() const { return m_bSnapToGrid; }
    void SetSnapToGrid(bool snap) { m_bSnapToGrid = snap; }

    float GetSnapIntervalUnit() const { return m_snapIntervalUnit; }
    void SetSnapIntervalUnit(float val) { if (val > 0.0f) m_snapIntervalUnit = val; }

    // Helper functions in pixels
    float GetSubGridStepPx() const { return m_subGridStepUnit * GetPixelsPerUnit(m_unit); }
    float GetMainGridStepPx() const { return m_mainGridStepUnit * GetPixelsPerUnit(m_unit); }
    float GetSnapIntervalPx() const { return m_snapIntervalUnit * GetPixelsPerUnit(m_unit); }

    Point2D SnapPoint(Point2D pt) const
    {
        if (!m_bSnapToGrid) return pt;
        float snapPx = GetSnapIntervalPx();
        if (snapPx <= 0.0f) return pt;

        float snappedX = roundf(pt.x / snapPx) * snapPx;
        float snappedY = roundf(pt.y / snapPx) * snapPx;
        return Point2D(snappedX, snappedY);
    }
};

#endif // GRID_SETTINGS_H
