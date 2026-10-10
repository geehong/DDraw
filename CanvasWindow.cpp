#include "CanvasWindow.h"
#include <windowsx.h>

#pragma comment(lib, "d2d1.lib")

CanvasWindow::CanvasWindow(ToolManager& toolMgr)
    : m_editorController(m_document, m_commandHistory, toolMgr)
{
}

CanvasWindow::~CanvasWindow()
{
    m_renderContext.DiscardDeviceResources();
}

void CanvasWindow::Update(float deltaTime)
{
    (void)deltaTime;
}

void CanvasWindow::Render()
{
    HRESULT hr = m_renderContext.CreateDeviceResources(m_hWnd);
    if (FAILED(hr)) return;

    ID2D1HwndRenderTarget* pTarget = m_renderContext.GetRenderTarget();
    if (!pTarget) return;

    RECT crt;
    GetClientRect(m_hWnd, &crt);
    float width = static_cast<float>(crt.right);
    float height = static_cast<float>(crt.bottom);

    pTarget->BeginDraw();
    pTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    pTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    m_gridRenderer.Render(m_renderContext, m_editorController.GetGridSettings(), width, height);

    m_renderer.RenderDocument(m_renderContext, m_document);

    const IShape* selectedShape = m_editorController.GetSelectionManager().GetSelectedShape();
    int activeHandle = m_editorController.GetActiveHandleIndex();
    int activeVertex = m_editorController.GetActiveVertexIndex();

    if (selectedShape)
    {
        m_renderer.RenderSelectionTracker(m_renderContext, selectedShape, activeHandle, activeVertex);
    }

    MouseState mouseState = m_editorController.GetMouseState();
    ToolType activeTool = m_editorController.GetToolManager().GetCurrentTool();
    if (activeTool == ToolType::Polygon && m_editorController.IsBuildingPolygon())
    {
        m_renderer.RenderPolygonPreview(m_renderContext, m_editorController.GetPolygonPoints(), mouseState.currentPos);
    }
    else
    {
        m_renderer.RenderPreview(m_renderContext, activeTool, mouseState.dragStartPos, mouseState.currentPos, mouseState.bIsDragging, selectedShape, activeHandle);
    }

    hr = pTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET)
    {
        m_renderContext.DiscardDeviceResources();
    }
}

LRESULT CanvasWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
        Render();
        ValidateRect(m_hWnd, NULL);
        return 0;

    case WM_SIZE:
        if (m_renderContext.GetRenderTarget())
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            m_renderContext.GetRenderTarget()->Resize(D2D1::SizeU(width, height));
        }
        return 0;

    case WM_LBUTTONDOWN:
    {
        Point2D pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        m_editorController.OnLButtonDown(pt, m_hWnd);
        InvalidateRect(m_hWnd, NULL, FALSE);
        
        HWND hParent = GetParent(m_hWnd);
        if (hParent)
        {
            PostMessage(hParent, WM_USER + 100, 0, 0);
        }
        return 0;
    }

    case WM_LBUTTONDBLCLK:
    {
        Point2D pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        m_editorController.OnLButtonDblClk(pt);
        InvalidateRect(m_hWnd, NULL, FALSE);

        HWND hParent = GetParent(m_hWnd);
        if (hParent)
        {
            PostMessage(hParent, WM_USER + 100, 0, 0);
        }
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        Point2D pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        m_editorController.OnMouseMove(pt);
        if (m_editorController.GetMouseState().bLButtonDown || m_editorController.IsBuildingPolygon())
        {
            InvalidateRect(m_hWnd, NULL, FALSE);
        }

        HWND hParent = GetParent(m_hWnd);
        if (hParent)
        {
            PostMessage(hParent, WM_USER + 100, (WPARAM)(int)pt.x, (LPARAM)(int)pt.y);
        }
        return 0;
    }

    case WM_LBUTTONUP:
    {
        Point2D pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        m_editorController.OnLButtonUp(pt, m_hWnd);
        InvalidateRect(m_hWnd, NULL, FALSE);
        
        // Notify parent MainWindow to refresh title bar asterisk (*), toolbar button checks, and status bar
        HWND hParent = GetParent(m_hWnd);
        if (hParent)
        {
            PostMessage(hParent, WM_COMMAND, MAKEWPARAM(0, 9999), 0);
            PostMessage(hParent, WM_USER + 101, 0, 0);
            PostMessage(hParent, WM_USER + 100, 0, 0);
        }
        return 0;
    }

    case WM_CAPTURECHANGED:
        m_editorController.OnCaptureChanged();
        InvalidateRect(m_hWnd, NULL, FALSE);
        return 0;

    case WM_CANCELMODE:
        m_editorController.OnCancelMode(m_hWnd);
        InvalidateRect(m_hWnd, NULL, FALSE);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            m_editorController.OnEscapeKey(m_hWnd);
            InvalidateRect(m_hWnd, NULL, FALSE);
        }
        else if (wParam == VK_RETURN)
        {
            m_editorController.OnEnterKey();
            InvalidateRect(m_hWnd, NULL, FALSE);
        }
        else if (wParam == VK_DELETE)
        {
            IShape* pSelected = m_editorController.GetSelectionManager().GetSelectedShape();
            if (pSelected)
            {
                m_commandHistory.ExecuteCommand(std::make_unique<DeleteShapeCommand>(
                    m_document, pSelected
                ));
                m_editorController.GetSelectionManager().ClearSelection();
                InvalidateRect(m_hWnd, NULL, FALSE);
                HWND hParent = GetParent(m_hWnd);
                if (hParent) PostMessage(hParent, WM_COMMAND, MAKEWPARAM(0, 9999), 0);
            }
        }
        else if (wParam == 'Z' && GetKeyState(VK_CONTROL) < 0)
        {
            m_commandHistory.Undo();
            InvalidateRect(m_hWnd, NULL, FALSE);
            HWND hParent = GetParent(m_hWnd);
            if (hParent) PostMessage(hParent, WM_COMMAND, MAKEWPARAM(0, 9999), 0);
        }
        else if (wParam == 'Y' && GetKeyState(VK_CONTROL) < 0)
        {
            m_commandHistory.Redo();
            InvalidateRect(m_hWnd, NULL, FALSE);
            HWND hParent = GetParent(m_hWnd);
            if (hParent) PostMessage(hParent, WM_COMMAND, MAKEWPARAM(0, 9999), 0);
        }
        return 0;

    case WM_RBUTTONDOWN:
    {
        Point2D pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        m_editorController.OnRButtonDown(pt);
        return 0;
    }

    case WM_RBUTTONUP:
    {
        Point2D pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        m_editorController.OnRButtonUp(pt);
        return 0;
    }

    case WM_MBUTTONDOWN:
    {
        Point2D pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        m_editorController.OnMButtonDown(pt);
        return 0;
    }

    case WM_MBUTTONUP:
    {
        Point2D pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        m_editorController.OnMButtonUp(pt);
        return 0;
    }

    case WM_MOUSELEAVE:
        m_editorController.OnMouseLeave();
        return 0;

    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT)
        {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(m_hWnd, &pt);
            HCURSOR hCursor = m_editorController.GetCursorForPosition(Point2D(static_cast<float>(pt.x), static_cast<float>(pt.y)));
            SetCursor(hCursor);
            return TRUE;
        }
        break;

    case WM_ERASEBKGND:
        return 1;
    }

    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}