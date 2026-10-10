#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "../Shapes/Shape.h"
#include <vector>
#include <memory>

class Document
{
private:
    std::vector<std::unique_ptr<IShape>> m_shapes;

public:
    Document() {}
    ~Document() {}

    Document(const Document& other)
    {
        for (const auto& shape : other.m_shapes)
        {
            if (shape)
            {
                m_shapes.push_back(shape->Clone());
            }
        }
    }

    Document& operator=(const Document& other)
    {
        if (this != &other)
        {
            m_shapes.clear();
            for (const auto& shape : other.m_shapes)
            {
                if (shape)
                {
                    m_shapes.push_back(shape->Clone());
                }
            }
        }
        return *this;
    }

    void AddShape(std::unique_ptr<IShape> shape)
    {
        if (shape)
        {
            m_shapes.push_back(std::move(shape));
        }
    }

    const std::vector<std::unique_ptr<IShape>>& GetShapes() const { return m_shapes; }

    IShape* HitTest(Point2D pt)
    {
        // 역순 검색 (가장 나중에 그려진 위쪽 도형부터 선택)
        for (auto it = m_shapes.rbegin(); it != m_shapes.rend(); ++it)
        {
            if ((*it) && (*it)->HitTest(pt))
            {
                return it->get();
            }
        }
        return nullptr;
    }

    std::unique_ptr<IShape> RemoveShapeAt(size_t index)
    {
        if (index >= m_shapes.size()) return nullptr;
        auto shape = std::move(m_shapes[index]);
        m_shapes.erase(m_shapes.begin() + index);
        return shape;
    }

    void InsertShapeAt(size_t index, std::unique_ptr<IShape> shape)
    {
        if (!shape) return;
        if (index >= m_shapes.size())
        {
            m_shapes.push_back(std::move(shape));
        }
        else
        {
            m_shapes.insert(m_shapes.begin() + index, std::move(shape));
        }
    }

    size_t FindShapeIndex(const IShape* shape) const
    {
        for (size_t i = 0; i < m_shapes.size(); ++i)
        {
            if (m_shapes[i].get() == shape) return i;
        }
        return static_cast<size_t>(-1);
    }

    void Clear()
    {
        m_shapes.clear();
    }

    size_t GetShapeCount() const { return m_shapes.size(); }
};

#endif // DOCUMENT_H
