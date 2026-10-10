#ifndef ELLIPSE_SHAPE_H
#define ELLIPSE_SHAPE_H

#include "Shape.h"
#include <cmath>

class EllipseShape : public IShape
{
private:
    Ellipse2D m_ellipse;

public:
    EllipseShape(Ellipse2D ellipse,
                 Color strokeColor = Color::Black(),
                 float strokeWidth = 2.0f,
                 bool bFilled = false,
                 Color fillColor = Color::White())
        : IShape(strokeColor, strokeWidth, bFilled, fillColor), m_ellipse(ellipse) {}

    std::unique_ptr<IShape> Clone() const override
    {
        return std::make_unique<EllipseShape>(*this);
    }

    bool HitTest(Point2D pt) override
    {
        if (m_ellipse.radiusX <= 0.0f || m_ellipse.radiusY <= 0.0f) return false;

        float dx = (pt.x - m_ellipse.center.x) / m_ellipse.radiusX;
        float dy = (pt.y - m_ellipse.center.y) / m_ellipse.radiusY;

        float val = dx * dx + dy * dy;
        if (m_bFilled)
        {
            return val <= 1.0f;
        }
        else
        {
            // Outline tolerance click test
            float outerR = 1.0f + (m_strokeWidth + 5.0f) / m_ellipse.radiusX;
            float innerR = 1.0f - (m_strokeWidth + 5.0f) / m_ellipse.radiusX;
            if (innerR < 0.0f) innerR = 0.0f;
            return (val <= outerR * outerR && val >= innerR * innerR) || (val <= 1.0f);
        }
    }

    void Move(float dx, float dy) override
    {
        m_ellipse.center.x += dx;
        m_ellipse.center.y += dy;
    }

    void Resize(int handleIndex, float dx, float dy) override
    {
        float left = m_ellipse.center.x - m_ellipse.radiusX;
        float right = m_ellipse.center.x + m_ellipse.radiusX;
        float top = m_ellipse.center.y - m_ellipse.radiusY;
        float bottom = m_ellipse.center.y + m_ellipse.radiusY;

        switch (handleIndex)
        {
        case 0: left += dx; top += dy; break;
        case 1: top += dy; break;
        case 2: right += dx; top += dy; break;
        case 3: left += dx; break;
        case 4: right += dx; break;
        case 5: left += dx; bottom += dy; break;
        case 6: bottom += dy; break;
        case 7: right += dx; bottom += dy; break;
        }

        m_ellipse.center.x = (left + right) / 2.0f;
        m_ellipse.center.y = (top + bottom) / 2.0f;
        m_ellipse.radiusX = fabsf(right - left) / 2.0f;
        m_ellipse.radiusY = fabsf(bottom - top) / 2.0f;
    }

    Rect2D GetBoundingBox() const override
    {
        return Rect2D(
            m_ellipse.center.x - m_ellipse.radiusX,
            m_ellipse.center.y - m_ellipse.radiusY,
            m_ellipse.center.x + m_ellipse.radiusX,
            m_ellipse.center.y + m_ellipse.radiusY
        );
    }

    ShapeType GetType() const override { return ShapeType::Ellipse; }

    Ellipse2D GetEllipse() const { return m_ellipse; }
    void SetEllipse(Ellipse2D ellipse) { m_ellipse = ellipse; }
};

#endif // ELLIPSE_SHAPE_H
