#ifndef TOOL_MANAGER_H
#define TOOL_MANAGER_H

#include "../Common/Types.h"

enum class ToolType
{
    Select,
    Rectangle,
    Ellipse,
    Line,
    Polygon,
    Text
};

class ToolManager
{
private:
    ToolType m_currentTool;
    Color    m_strokeColor;
    Color    m_fillColor;
    float    m_strokeWidth;
    bool     m_bFilled;

public:
    ToolManager()
        : m_currentTool(ToolType::Select),
          m_strokeColor(Color::Black()),
          m_fillColor(Color::White()),
          m_strokeWidth(2.0f),
          m_bFilled(false) {}

    ToolType GetCurrentTool() const { return m_currentTool; }
    void SetCurrentTool(ToolType tool) { m_currentTool = tool; }

    Color GetStrokeColor() const { return m_strokeColor; }
    void SetStrokeColor(Color color) { m_strokeColor = color; }

    Color GetFillColor() const { return m_fillColor; }
    void SetFillColor(Color color) { m_fillColor = color; }

    float GetStrokeWidth() const { return m_strokeWidth; }
    void SetStrokeWidth(float width) { m_strokeWidth = width; }

    bool IsFilled() const { return m_bFilled; }
    void SetFilled(bool filled) { m_bFilled = filled; }
};

#endif // TOOL_MANAGER_H
