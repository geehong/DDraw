#ifndef GRID_SETTINGS_H
#define GRID_SETTINGS_H

#include "../Common/Types.h"

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
    
    // Main Grid Properties
    float    m_mainGridStepUnit;  // Main grid step in unit (e.g. 50 pt / 50 mm)
    float    m_mainGridLineWidth; // Main grid line width (px) e.g. 1.0 px
    Color    m_mainGridColor;     // Main grid RGB color
    float    m_mainGridOpacity;   // Main grid opacity (0.0 ~ 1.0)
    
    // Sub Grid Properties
    int      m_subDivisions;      // Number of subdivisions inside 1 Main Grid cell (e.g. 5 splits = sub-grid step is 50/5 = 10 pt)
    float    m_subGridLineWidth;  // Sub grid line width (px) e.g. 0.5 px
    Color    m_subGridColor;      // Sub grid RGB color
    float    m_subGridOpacity;    // Sub grid opacity (0.0 ~ 1.0)
    
    bool     m_bSnapToGrid;       // Snap to sub-grid intersection

public:
    GridSettings()
        : m_unit(GridUnit::Point),
          m_bShowGrid(true),
          m_mainGridStepUnit(50.0f),
          m_mainGridLineWidth(1.0f),
          m_mainGridColor(0.20f, 0.25f, 0.35f, 1.0f),
          m_mainGridOpacity(0.95f),
          m_subDivisions(5),
          m_subGridLineWidth(0.5f),
          m_subGridColor(0.50f, 0.55f, 0.65f, 1.0f),
          m_subGridOpacity(0.50f),
          m_bSnapToGrid(true) {}

    GridUnit GetUnit() const { return m_unit; }
    void SetUnit(GridUnit unit)
    {
        m_unit = unit;
        switch (unit)
        {
        case GridUnit::Point:
            m_mainGridStepUnit = 50.0f;
            m_subDivisions = 5;       // 50 / 5 = 10 pt per sub grid
            break;
        case GridUnit::MM:
            m_mainGridStepUnit = 50.0f;
            m_subDivisions = 5;       // 50 / 5 = 10 mm per sub grid
            break;
        case GridUnit::CM:
            m_mainGridStepUnit = 5.0f;
            m_subDivisions = 5;       // 5 / 5 = 1 cm per sub grid
            break;
        case GridUnit::M:
            m_mainGridStepUnit = 1.0f;
            m_subDivisions = 10;      // 1 / 10 = 0.1 m per sub grid
            break;
        case GridUnit::Inch:
            m_mainGridStepUnit = 1.0f;
            m_subDivisions = 8;       // 1 / 8 = 0.125 in per sub grid
            break;
        }
    }

    bool IsGridVisible() const { return m_bShowGrid; }
    void SetGridVisible(bool visible) { m_bShowGrid = visible; }

    // Main Grid Getters & Setters
    float GetMainGridStepUnit() const { return m_mainGridStepUnit; }
    void SetMainGridStepUnit(float step) { if (step > 0.0f) m_mainGridStepUnit = step; }

    float GetMainGridLineWidth() const { return m_mainGridLineWidth; }
    void SetMainGridLineWidth(float width) { if (width > 0.0f) m_mainGridLineWidth = width; }

    Color GetMainGridColor() const { return m_mainGridColor; }
    void SetMainGridColor(const Color& color) { m_mainGridColor = color; }

    float GetMainGridOpacity() const { return m_mainGridOpacity; }
    void SetMainGridOpacity(float opacity) { m_mainGridOpacity = (opacity < 0.0f) ? 0.0f : ((opacity > 1.0f) ? 1.0f : opacity); }

    // Sub Grid Getters & Setters
    int GetSubDivisions() const { return m_subDivisions; }
    void SetSubDivisions(int div) { if (div > 0) m_subDivisions = div; }

    float GetSubGridLineWidth() const { return m_subGridLineWidth; }
    void SetSubGridLineWidth(float width) { if (width > 0.0f) m_subGridLineWidth = width; }

    Color GetSubGridColor() const { return m_subGridColor; }
    void SetSubGridColor(const Color& color) { m_subGridColor = color; }

    float GetSubGridOpacity() const { return m_subGridOpacity; }
    void SetSubGridOpacity(float opacity) { m_subGridOpacity = (opacity < 0.0f) ? 0.0f : ((opacity > 1.0f) ? 1.0f : opacity); }

    // Calculated Sub Grid Step in Unit = MainGridStepUnit / SubDivisions
    float GetSubGridStepUnit() const
    {
        return (m_subDivisions > 0) ? (m_mainGridStepUnit / static_cast<float>(m_subDivisions)) : m_mainGridStepUnit;
    }

    bool IsSnapToGrid() const { return m_bSnapToGrid; }
    void SetSnapToGrid(bool snap) { m_bSnapToGrid = snap; }

    // Helper functions in pixels
    float GetMainGridStepPx() const { return m_mainGridStepUnit * GetPixelsPerUnit(m_unit); }
    float GetSubGridStepPx() const { return GetSubGridStepUnit() * GetPixelsPerUnit(m_unit); }

    // Snap to sub-grid step (in pixels)
    float GetSnapIntervalPx() const { return GetSubGridStepPx(); }

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
