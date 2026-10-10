#ifndef POLYGON_SHAPE_H
#define POLYGON_SHAPE_H

#include "Shape.h"
#include <vector>
#include <algorithm>
#include <cmath>

class PolygonShape : public IShape
{
private:
    std::vector<Point2D> m_points;

public:
    PolygonShape(const std::vector<Point2D>& points,
                 Color strokeColor = Color::Black(),
                 float strokeWidth = 2.0f,
                 bool bFilled = false,
                 Color fillColor = Color::White())
        : IShape(strokeColor, strokeWidth, bFilled, fillColor), m_points(points) {}

    std::unique_ptr<IShape> Clone() const override
    {
        return std::make_unique<PolygonShape>(*this);
    }

    const std::vector<Point2D>& GetPoints() const { return m_points; }
    void SetPoints(const std::vector<Point2D>& pts) { m_points = pts; }

    bool HitTest(Point2D pt) override
    {
        if (m_points.size() < 2) return false;

        Rect2D bbox = GetBoundingBox();
        const float margin = 10.0f;
        if (pt.x < bbox.left - margin || pt.x > bbox.right + margin ||
            pt.y < bbox.top - margin || pt.y > bbox.bottom + margin)
        {
            return false;
        }

        // 1. Check if near any vertex point
        if (HitTestVertex(pt, 10.0f) != -1)
        {
            return true;
        }

        // 2. Check if near any edge line
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            size_t j = (i + 1) % m_points.size();
            Point2D p1 = m_points[i];
            Point2D p2 = m_points[j];
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float lenSq = dx * dx + dy * dy;
            if (lenSq > 0.001f)
            {
                float t = ((pt.x - p1.x) * dx + (pt.y - p1.y) * dy) / lenSq;
                t = (std::max)(0.0f, (std::min)(1.0f, t));
                float projX = p1.x + t * dx;
                float projY = p1.y + t * dy;
                float distSq = (pt.x - projX) * (pt.x - projX) + (pt.y - projY) * (pt.y - projY);
                if (distSq <= (m_strokeWidth + 8.0f) * (m_strokeWidth + 8.0f))
                {
                    return true;
                }
            }
        }

        // 3. Ray casting for filled polygon interior
        if (m_bFilled && m_points.size() >= 3)
        {
            bool inside = false;
            size_t count = m_points.size();
            for (size_t i = 0, j = count - 1; i < count; j = i++)
            {
                if (((m_points[i].y > pt.y) != (m_points[j].y > pt.y)) &&
                    (pt.x < (m_points[j].x - m_points[i].x) * (pt.y - m_points[i].y) / (m_points[j].y - m_points[i].y) + m_points[i].x))
                {
                    inside = !inside;
                }
            }
            return inside;
        }

        return true;
    }

    int HitTestVertex(Point2D pt, float tolerance = 8.0f) const
    {
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            if (fabsf(pt.x - m_points[i].x) <= tolerance && fabsf(pt.y - m_points[i].y) <= tolerance)
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void MoveVertex(size_t index, float dx, float dy)
    {
        if (index < m_points.size())
        {
            m_points[index].x += dx;
            m_points[index].y += dy;
        }
    }

    void SetVertex(size_t index, Point2D pt)
    {
        if (index < m_points.size())
        {
            m_points[index] = pt;
        }
    }

    void AddVertex(size_t afterIndex, Point2D pt)
    {
        if (afterIndex < m_points.size())
        {
            m_points.insert(m_points.begin() + afterIndex + 1, pt);
        }
        else
        {
            m_points.push_back(pt);
        }
    }

    void RemoveVertex(size_t index)
    {
        if (m_points.size() > 3 && index < m_points.size())
        {
            m_points.erase(m_points.begin() + index);
        }
    }

    void Move(float dx, float dy) override
    {
        for (auto& pt : m_points)
        {
            pt.x += dx;
            pt.y += dy;
        }
    }

    void Resize(int handleIndex, float dx, float dy) override
    {
        if (m_points.empty()) return;
        Rect2D bbox = GetBoundingBox();
        float width = bbox.right - bbox.left;
        float height = bbox.bottom - bbox.top;
        if (width <= 0.001f || height <= 0.001f) return;

        float newLeft = bbox.left;
        float newRight = bbox.right;
        float newTop = bbox.top;
        float newBottom = bbox.bottom;

        switch (handleIndex)
        {
        case 0: newLeft += dx; newTop += dy; break;
        case 1: newTop += dy; break;
        case 2: newRight += dx; newTop += dy; break;
        case 3: newLeft += dx; break;
        case 4: newRight += dx; break;
        case 5: newLeft += dx; newBottom += dy; break;
        case 6: newBottom += dy; break;
        case 7: newRight += dx; newBottom += dy; break;
        }

        float scaleX = (newRight - newLeft) / width;
        float scaleY = (newBottom - newTop) / height;

        for (auto& pt : m_points)
        {
            pt.x = newLeft + (pt.x - bbox.left) * scaleX;
            pt.y = newTop + (pt.y - bbox.top) * scaleY;
        }
    }

    Rect2D GetBoundingBox() const override
    {
        if (m_points.empty()) return Rect2D(0, 0, 0, 0);

        float minX = m_points[0].x;
        float maxX = m_points[0].x;
        float minY = m_points[0].y;
        float maxY = m_points[0].y;

        for (const auto& pt : m_points)
        {
            if (pt.x < minX) minX = pt.x;
            if (pt.x > maxX) maxX = pt.x;
            if (pt.y < minY) minY = pt.y;
            if (pt.y > maxY) maxY = pt.y;
        }
        return Rect2D(minX, minY, maxX, maxY);
    }

    ShapeType GetType() const override
    {
        return ShapeType::Freehand;
    }
};

#endif // POLYGON_SHAPE_H
