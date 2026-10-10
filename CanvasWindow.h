#ifndef CANVAS_WINDOW_H
#define CANVAS_WINDOW_H

#include "BaseWindow.h"
#include "Model/Document.h"
#include "Commands/CommandHistory.h"
#include "Rendering/RenderContext.h"
#include "Rendering/GridRenderer.h"
#include "Rendering/Direct2DRenderer.h"
#include "Editing/EditorController.h"
#include "Animation/PhysicsObject.h"

class CanvasWindow : public BaseWindow
{
private:
    // Core Subsystems
    RenderContext    m_renderContext;
    GridRenderer     m_gridRenderer;
    Direct2DRenderer m_renderer;
    Document         m_document;
    CommandHistory   m_commandHistory;
    EditorController m_editorController;
    PhysicsObject    m_physicsObject;

public:
    CanvasWindow(ToolManager& toolMgr);
    virtual ~CanvasWindow();

    Document& GetDocument() { return m_document; }
    CommandHistory& GetCommandHistory() { return m_commandHistory; }
    EditorController& GetEditorController() { return m_editorController; }

    // Active Render Loop: Separated Update & Render
    void Update(float deltaTime);
    void Render();

protected:
    virtual PCWSTR GetClassName() const override { return L"D2DCanvasWindow"; }
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
    void OnPaint();
    void OnResize(UINT width, UINT height);
};

#endif // CANVAS_WINDOW_H
