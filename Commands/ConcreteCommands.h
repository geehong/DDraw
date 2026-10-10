#ifndef CONCRETE_COMMANDS_H
#define CONCRETE_COMMANDS_H

#include "ICommand.h"
#include "../Model/Document.h"
#include "../Shapes/Shape.h"
#include "../Shapes/PolygonShape.h"
#include <memory>

class AddShapeCommand : public ICommand
{
private:
    Document& m_document;
    std::unique_ptr<IShape> m_shape;
    IShape* m_pRawShape;

public:
    AddShapeCommand(Document& doc, std::unique_ptr<IShape> shape)
        : m_document(doc), m_shape(std::move(shape)), m_pRawShape(nullptr)
    {
        m_pRawShape = m_shape.get();
    }

    virtual void Execute() override
    {
        if (m_shape)
        {
            m_document.AddShape(std::move(m_shape));
        }
    }

    virtual void Undo() override
    {
        size_t idx = m_document.FindShapeIndex(m_pRawShape);
        if (idx != static_cast<size_t>(-1))
        {
            m_shape = m_document.RemoveShapeAt(idx);
        }
    }
};

class MoveShapeCommand : public ICommand
{
private:
    IShape* m_shape;
    float m_dx;
    float m_dy;

public:
    MoveShapeCommand(IShape* shape, float dx, float dy)
        : m_shape(shape), m_dx(dx), m_dy(dy) {}

    virtual void Execute() override
    {
        if (m_shape)
        {
            m_shape->Move(m_dx, m_dy);
        }
    }

    virtual void Undo() override
    {
        if (m_shape)
        {
            m_shape->Move(-m_dx, -m_dy);
        }
    }
};

class ResizeShapeCommand : public ICommand
{
private:
    IShape* m_shape;
    int m_handleIndex;
    float m_dx;
    float m_dy;

public:
    ResizeShapeCommand(IShape* shape, int handleIndex, float dx, float dy)
        : m_shape(shape), m_handleIndex(handleIndex), m_dx(dx), m_dy(dy) {}

    virtual void Execute() override
    {
        if (m_shape)
        {
            m_shape->Resize(m_handleIndex, m_dx, m_dy);
        }
    }

    virtual void Undo() override
    {
        if (m_shape)
        {
            m_shape->Resize(m_handleIndex, -m_dx, -m_dy);
        }
    }
};

class MoveVertexCommand : public ICommand
{
private:
    PolygonShape* m_polygon;
    size_t m_vertexIndex;
    float m_dx;
    float m_dy;

public:
    MoveVertexCommand(PolygonShape* poly, size_t index, float dx, float dy)
        : m_polygon(poly), m_vertexIndex(index), m_dx(dx), m_dy(dy) {}

    virtual void Execute() override
    {
        if (m_polygon)
        {
            m_polygon->MoveVertex(m_vertexIndex, m_dx, m_dy);
        }
    }

    virtual void Undo() override
    {
        if (m_polygon)
        {
            m_polygon->MoveVertex(m_vertexIndex, -m_dx, -m_dy);
        }
    }
};

class DeleteShapeCommand : public ICommand
{
private:
    Document& m_document;
    std::unique_ptr<IShape> m_shape;
    IShape* m_pRawShape;
    size_t m_originalIndex;

public:
    DeleteShapeCommand(Document& doc, IShape* targetShape)
        : m_document(doc), m_pRawShape(targetShape), m_originalIndex(static_cast<size_t>(-1)) {}

    virtual void Execute() override
    {
        if (!m_pRawShape) return;
        m_originalIndex = m_document.FindShapeIndex(m_pRawShape);
        if (m_originalIndex != static_cast<size_t>(-1))
        {
            m_shape = m_document.RemoveShapeAt(m_originalIndex);
        }
    }

    virtual void Undo() override
    {
        if (m_shape && m_originalIndex != static_cast<size_t>(-1))
        {
            m_document.InsertShapeAt(m_originalIndex, std::move(m_shape));
        }
    }
};

class ChangeStyleCommand : public ICommand
{
private:
    IShape* m_shape;
    Color m_oldStrokeColor, m_newStrokeColor;
    Color m_oldFillColor, m_newFillColor;
    float m_oldStrokeWidth, m_newStrokeWidth;
    bool  m_oldFilled, m_newFilled;

public:
    ChangeStyleCommand(IShape* shape,
                       Color newStroke, Color newFill,
                       float newWidth, bool newFilled)
        : m_shape(shape),
          m_newStrokeColor(newStroke), m_newFillColor(newFill),
          m_newStrokeWidth(newWidth), m_newFilled(newFilled)
    {
        if (m_shape)
        {
            m_oldStrokeColor = m_shape->GetStrokeColor();
            m_oldFillColor = m_shape->GetFillColor();
            m_oldStrokeWidth = m_shape->GetStrokeWidth();
            m_oldFilled = m_shape->IsFilled();
        }
    }

    virtual void Execute() override
    {
        if (m_shape)
        {
            m_shape->SetStrokeColor(m_newStrokeColor);
            m_shape->SetFillColor(m_newFillColor);
            m_shape->SetStrokeWidth(m_newStrokeWidth);
            m_shape->SetFilled(m_newFilled);
        }
    }

    virtual void Undo() override
    {
        if (m_shape)
        {
            m_shape->SetStrokeColor(m_oldStrokeColor);
            m_shape->SetFillColor(m_oldFillColor);
            m_shape->SetStrokeWidth(m_oldStrokeWidth);
            m_shape->SetFilled(m_oldFilled);
        }
    }
};

#endif // CONCRETE_COMMANDS_H
