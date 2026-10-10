#ifndef LINE_SHAPE_H
#define LINE_SHAPE_H

#include "Shape.h"
#include <algorithm>
#include <cmath>

class LineShape : public IShape
{
private:
    Point2D m_startPoint;
    Point2D m_endPoint;

public:
    LineShape(Point2D start, Point2D end,
              Color strokeColor = Color::Black(),
              float strokeWidth = 2.0f)
        : IShape(strokeColor, strokeWidth, false), m_startPoint(start), m_endPoint(end) {}

    std::unique_ptr<IShape> Clone() const override
    {
        return std::make_unique<LineShape>(*this);
    }

    bool HitTest(Point2D pt) override
    {
        float dx = m_endPoint.x - m_startPoint.x;
        float dy = m_endPoint.y - m_startPoint.y;
        float lenSq = dx * dx + dy * dy;
        if (lenSq == 0.0f) return false;

        float t = ((pt.x - m_startPoint.x) * dx + (pt.y - m_startPoint.y) * dy) / lenSq;
        t = (std::max)(0.0f, (std::min)(1.0f, t));

        float projX = m_startPoint.x + t * dx;
        float projY = m_startPoint.y + t * dy;

        float distSq = (pt.x - projX) * (pt.x - projX) + (pt.y - projY) * (pt.y - projY);
        return distSq <= (m_strokeWidth + 5.0f) * (m_strokeWidth + 5.0f);
    }

    void Move(float dx, float dy) override
    {
        m_startPoint.x += dx;
        m_startPoint.y += dy;
        m_endPoint.x += dx;
        m_endPoint.y += dy;
    }

    void Resize(int handleIndex, float dx, float dy) override
    {
        if (handleIndex == 0 || handleIndex == 1 || handleIndex == 3 || handleIndex == 5)
        {
            m_startPoint.x += dx;
            m_startPoint.y += dy;
        }
        else
        {
            m_endPoint.x += dx;
            m_endPoint.y += dy;
        }
    }

    Rect2D GetBoundingBox() const override
    {
        return Rect2D(
            (std::min)(m_startPoint.x, m_endPoint.x),
            (std::min)(m_startPoint.y, m_endPoint.y),
            (std::max)(m_startPoint.x, m_endPoint.x),
            (std::max)(m_startPoint.y, m_endPoint.y)
        );
    }

    ShapeType GetType() const override { return ShapeType::Line; }

    Point2D GetStartPoint() const { return m_startPoint; }
    void SetStartPoint(Point2D start) { m_startPoint = start; }

    Point2D GetEndPoint() const { return m_endPoint; }
    void SetEndPoint(Point2D end) { m_endPoint = end; }
};

#endif // LINE_SHAPE_H
