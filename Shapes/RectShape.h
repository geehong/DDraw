#ifndef RECT_SHAPE_H
#define RECT_SHAPE_H

#include "Shape.h"
#include <algorithm>

class RectShape : public IShape
{
private:
    Rect2D m_rect;

public:
    RectShape(Rect2D rect,
              Color strokeColor = Color::Black(),
              float strokeWidth = 2.0f,
              bool bFilled = false,
              Color fillColor = Color::White())
        : IShape(strokeColor, strokeWidth, bFilled, fillColor), m_rect(rect) {}

    std::unique_ptr<IShape> Clone() const override
    {
        return std::make_unique<RectShape>(*this);
    }

    bool HitTest(Point2D pt) override
    {
        float left = (std::min)(m_rect.left, m_rect.right);
        float right = (std::max)(m_rect.left, m_rect.right);
        float top = (std::min)(m_rect.top, m_rect.bottom);
        float bottom = (std::max)(m_rect.top, m_rect.bottom);

        return (pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom);
    }

    void Move(float dx, float dy) override
    {
        m_rect.left += dx;
        m_rect.right += dx;
        m_rect.top += dy;
        m_rect.bottom += dy;
    }

    void Resize(int handleIndex, float dx, float dy) override
    {
        // handleIndex: 0:TL, 1:TC, 2:TR, 3:ML, 4:MR, 5:BL, 6:BC, 7:BR
        switch (handleIndex)
        {
        case 0: m_rect.left += dx; m_rect.top += dy; break;
        case 1: m_rect.top += dy; break;
        case 2: m_rect.right += dx; m_rect.top += dy; break;
        case 3: m_rect.left += dx; break;
        case 4: m_rect.right += dx; break;
        case 5: m_rect.left += dx; m_rect.bottom += dy; break;
        case 6: m_rect.bottom += dy; break;
        case 7: m_rect.right += dx; m_rect.bottom += dy; break;
        }
    }

    Rect2D GetBoundingBox() const override
    {
        float left = (std::min)(m_rect.left, m_rect.right);
        float right = (std::max)(m_rect.left, m_rect.right);
        float top = (std::min)(m_rect.top, m_rect.bottom);
        float bottom = (std::max)(m_rect.top, m_rect.bottom);
        return Rect2D(left, top, right, bottom);
    }

    ShapeType GetType() const override { return ShapeType::Rectangle; }

    Rect2D GetRect() const { return m_rect; }
    void SetRect(Rect2D rect) { m_rect = rect; }
};

#endif // RECT_SHAPE_H
