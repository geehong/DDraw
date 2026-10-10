#ifndef SELECTION_MANAGER_H
#define SELECTION_MANAGER_H

#include "../Shapes/Shape.h"
#include "../Shapes/PolygonShape.h"

class SelectionManager
{
private:
    IShape* m_pSelectedShape;

public:
    SelectionManager() : m_pSelectedShape(nullptr) {}

    void Select(IShape* shape) { m_pSelectedShape = shape; }
    void ClearSelection() { m_pSelectedShape = nullptr; }

    IShape* GetSelectedShape() const { return m_pSelectedShape; }
    bool HasSelection() const { return m_pSelectedShape != nullptr; }

    int HitTestVertex(Point2D pt) const
    {
        if (!m_pSelectedShape) return -1;
        if (m_pSelectedShape->GetType() == ShapeType::Freehand)
        {
            const auto* pPoly = static_cast<const PolygonShape*>(m_pSelectedShape);
            return pPoly->HitTestVertex(pt);
        }
        return -1;
    }

    int HitTestHandle(Point2D pt) const
    {
        if (!m_pSelectedShape) return -1;

        Rect2D bounds = m_pSelectedShape->GetBoundingBox();
        float left = bounds.left;
        float right = bounds.right;
        float top = bounds.top;
        float bottom = bounds.bottom;
        float midX = (left + right) / 2.0f;
        float midY = (top + bottom) / 2.0f;

        Point2D handles[8] = {
            {left, top}, {midX, top}, {right, top},
            {left, midY},             {right, midY},
            {left, bottom}, {midX, bottom}, {right, bottom}
        };

        const float tolerance = 7.0f; // 14x14px hit region
        for (int i = 0; i < 8; ++i)
        {
            if (fabsf(pt.x - handles[i].x) <= tolerance && fabsf(pt.y - handles[i].y) <= tolerance)
            {
                return i; // 0..7 Handle index
            }
        }
        return -1;
    }
};

#endif // SELECTION_MANAGER_H
