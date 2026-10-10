#ifndef EDITOR_CONTROLLER_H
#define EDITOR_CONTROLLER_H

#include "../Model/Document.h"
#include "../Commands/CommandHistory.h"
#include "../Commands/ConcreteCommands.h"
#include "../Shapes/RectShape.h"
#include "../Shapes/EllipseShape.h"
#include "../Shapes/LineShape.h"
#include "../Shapes/TextShape.h"
#include "../Shapes/PolygonShape.h"
#include "SelectionManager.h"
#include "ToolManager.h"
#include "GridSettings.h"
#include <windows.h>
#include <windowsx.h>
#include <cmath>
#include <vector>
#include <string>

struct MouseState
{
    Point2D currentPos;
    Point2D dragStartPos;
    bool bLButtonDown;
    bool bRButtonDown;
    bool bMButtonDown;
    bool bIsDragging;
    bool bInCanvas;

    MouseState()
        : currentPos(0, 0), dragStartPos(0, 0),
          bLButtonDown(false), bRButtonDown(false), bMButtonDown(false),
          bIsDragging(false), bInCanvas(false) {}
};

class EditorController
{
private:
    Document& m_document;
    CommandHistory& m_commandHistory;
    ToolManager& m_toolManager;
    SelectionManager m_selectionManager;
    GridSettings     m_gridSettings;
    MouseState m_mouseState;
    bool m_bCapturingMouse;
    int  m_activeHandleIndex;
    int  m_activeVertexIndex;
    std::vector<Point2D> m_polygonPoints;
    std::wstring m_lastStatusMessage;

public:
    EditorController(Document& doc, CommandHistory& cmdHistory, ToolManager& toolMgr)
        : m_document(doc), m_commandHistory(cmdHistory), m_toolManager(toolMgr),
          m_bCapturingMouse(false), m_activeHandleIndex(-1), m_activeVertexIndex(-1) {}

    SelectionManager& GetSelectionManager() { return m_selectionManager; }
    GridSettings& GetGridSettings() { return m_gridSettings; }
    const GridSettings& GetGridSettings() const { return m_gridSettings; }
    const MouseState& GetMouseState() const { return m_mouseState; }
    CommandHistory& GetCommandHistory() { return m_commandHistory; }
    ToolManager& GetToolManager() { return m_toolManager; }
    int GetActiveHandleIndex() const { return m_activeHandleIndex; }
    int GetActiveVertexIndex() const { return m_activeVertexIndex; }

    const std::wstring& GetLastStatusMessage() const { return m_lastStatusMessage; }
    void SetLastStatusMessage(const std::wstring& msg) { m_lastStatusMessage = msg; }

    bool IsBuildingPolygon() const { return !m_polygonPoints.empty(); }
    const std::vector<Point2D>& GetPolygonPoints() const { return m_polygonPoints; }

    HCURSOR GetCursorForPosition(Point2D pt) const
    {
        ToolType activeTool = m_toolManager.GetCurrentTool();
        if (activeTool != ToolType::Select)
        {
            return LoadCursor(NULL, IDC_CROSS);
        }

        if (m_selectionManager.HasSelection())
        {
            int vertex = m_selectionManager.HitTestVertex(pt);
            if (vertex != -1)
            {
                return LoadCursor(NULL, IDC_CROSS);
            }

            int handle = m_selectionManager.HitTestHandle(pt);
            switch (handle)
            {
            case 0:
            case 7:
                return LoadCursor(NULL, IDC_SIZENWSE);
            case 2:
            case 5:
                return LoadCursor(NULL, IDC_SIZENESW);
            case 1:
            case 6:
                return LoadCursor(NULL, IDC_SIZENS);
            case 3:
            case 4:
                return LoadCursor(NULL, IDC_SIZEWE);
            default:
                break;
            }

            IShape* hit = m_document.HitTest(pt);
            if (hit)
            {
                return LoadCursor(NULL, IDC_SIZEALL);
            }
        }

        return LoadCursor(NULL, IDC_ARROW);
    }

    void FinishPolygon()
    {
        if (m_polygonPoints.size() >= 3)
        {
            auto newShape = std::make_unique<PolygonShape>(
                m_polygonPoints,
                m_toolManager.GetStrokeColor(),
                m_toolManager.GetStrokeWidth(),
                m_toolManager.IsFilled(),
                m_toolManager.GetFillColor()
            );
            IShape* pRaw = newShape.get();
            m_commandHistory.ExecuteCommand(std::make_unique<AddShapeCommand>(
                m_document, std::move(newShape)
            ));
            // Selection points visible for completed polygon
            m_selectionManager.Select(pRaw);
            // Switch to select tool automatically upon polygon finish
            m_toolManager.SetCurrentTool(ToolType::Select);
        }
        m_polygonPoints.clear();
    }

    void OnLButtonDown(Point2D pt, HWND hWnd)
    {
        Point2D rawPt = pt;
        Point2D snapPt = m_gridSettings.SnapPoint(pt);

        m_mouseState.currentPos = snapPt;
        m_mouseState.dragStartPos = snapPt;
        m_mouseState.bLButtonDown = true;
        m_mouseState.bInCanvas = true;
        m_activeHandleIndex = -1;
        m_activeVertexIndex = -1;

        if (m_toolManager.GetCurrentTool() == ToolType::Polygon)
        {
            m_polygonPoints.push_back(snapPt);
            m_mouseState.bIsDragging = false;
        }
        else
        {
            m_polygonPoints.clear();
            if (m_toolManager.GetCurrentTool() == ToolType::Select)
            {
                if (m_selectionManager.HasSelection())
                {
                    int vertex = m_selectionManager.HitTestVertex(pt);
                    if (vertex != -1)
                    {
                        m_activeVertexIndex = vertex;
                        m_mouseState.bIsDragging = true;
                    }
                    else
                    {
                        int handle = m_selectionManager.HitTestHandle(pt);
                        if (handle != -1)
                        {
                            m_activeHandleIndex = handle;
                            m_mouseState.bIsDragging = true;
                        }
                    }
                }

                if (m_activeVertexIndex == -1 && m_activeHandleIndex == -1)
                {
                    IShape* hit = m_document.HitTest(pt);
                    if (hit)
                    {
                        m_selectionManager.Select(hit);
                        m_mouseState.bIsDragging = true;
                    }
                    else
                    {
                        m_selectionManager.ClearSelection();
                        m_mouseState.bIsDragging = false;
                    }
                }
            }
            else
            {
                m_selectionManager.ClearSelection();
                m_mouseState.bIsDragging = true;
            }
        }

        if (hWnd && !m_bCapturingMouse)
        {
            SetCapture(hWnd);
            m_bCapturingMouse = true;
        }
    }

    void OnLButtonDblClk(Point2D pt)
    {
        (void)pt;
        if (m_toolManager.GetCurrentTool() == ToolType::Polygon && IsBuildingPolygon())
        {
            if (m_polygonPoints.size() > 1)
            {
                Point2D last = m_polygonPoints.back();
                Point2D prev = m_polygonPoints[m_polygonPoints.size() - 2];
                float dx = fabsf(last.x - prev.x);
                float dy = fabsf(last.y - prev.y);
                if (dx < 5.0f && dy < 5.0f)
                {
                    m_polygonPoints.pop_back();
                }
            }
            FinishPolygon();
        }
    }

    void OnEnterKey()
    {
        if (IsBuildingPolygon())
        {
            FinishPolygon();
        }
    }

    void OnMouseMove(Point2D pt)
    {
        m_mouseState.currentPos = m_gridSettings.SnapPoint(pt);
        m_mouseState.bInCanvas = true;
    }

    void OnLButtonUp(Point2D pt, HWND hWnd)
    {
        Point2D snapPt = m_gridSettings.SnapPoint(pt);
        m_mouseState.currentPos = snapPt;
        m_mouseState.bLButtonDown = false;

        ToolType currentTool = m_toolManager.GetCurrentTool();

        if (currentTool == ToolType::Select)
        {
            if (m_mouseState.bIsDragging && m_selectionManager.HasSelection())
            {
                float dx = snapPt.x - m_mouseState.dragStartPos.x;
                float dy = snapPt.y - m_mouseState.dragStartPos.y;

                if (dx != 0.0f || dy != 0.0f)
                {
                    if (m_activeVertexIndex != -1)
                    {
                        auto* pPoly = static_cast<PolygonShape*>(m_selectionManager.GetSelectedShape());
                        if (pPoly && pPoly->GetType() == ShapeType::Freehand)
                        {
                            m_commandHistory.ExecuteCommand(std::make_unique<MoveVertexCommand>(
                                pPoly, m_activeVertexIndex, dx, dy
                            ));
                        }
                    }
                    else if (m_activeHandleIndex != -1)
                    {
                        IShape* pShape = m_selectionManager.GetSelectedShape();
                        if (pShape)
                        {
                            m_commandHistory.ExecuteCommand(std::make_unique<ResizeShapeCommand>(
                                pShape, m_activeHandleIndex, dx, dy
                            ));
                        }
                    }
                    else
                    {
                        m_commandHistory.ExecuteCommand(std::make_unique<MoveShapeCommand>(
                            m_selectionManager.GetSelectedShape(), dx, dy
                        ));
                    }
                }
            }
        }
        else if (currentTool == ToolType::Polygon)
        {
            m_mouseState.bIsDragging = false;
        }
        else
        {
            Point2D start = m_mouseState.dragStartPos;
            Point2D end = snapPt;

            float dxSq = (end.x - start.x) * (end.x - start.x);
            float dySq = (end.y - start.y) * (end.y - start.y);
            bool isClick = (dxSq + dySq <= 9.0f); // empty click down/up without drag

            if (isClick)
            {
                // Empty click on canvas while drawing tool is selected -> switch to Select tool and hit-test
                m_toolManager.SetCurrentTool(ToolType::Select);
                IShape* hit = m_document.HitTest(pt);
                if (hit)
                {
                    m_selectionManager.Select(hit);
                }
                else
                {
                    m_selectionManager.ClearSelection();
                }
            }
            else
            {
                // Dragging occurred -> create shape and keep shape tool active for next drawing
                std::unique_ptr<IShape> newShape = nullptr;

                switch (currentTool)
                {
                case ToolType::Rectangle:
                {
                    float left = (std::min)(start.x, end.x);
                    float right = (std::max)(start.x, end.x);
                    float top = (std::min)(start.y, end.y);
                    float bottom = (std::max)(start.y, end.y);

                    newShape = std::make_unique<RectShape>(
                        Rect2D(left, top, right, bottom),
                        m_toolManager.GetStrokeColor(),
                        m_toolManager.GetStrokeWidth(),
                        m_toolManager.IsFilled(),
                        m_toolManager.GetFillColor()
                    );
                    break;
                }

                case ToolType::Ellipse:
                {
                    float rx = fabsf(end.x - start.x) / 2.0f;
                    float ry = fabsf(end.y - start.y) / 2.0f;
                    Point2D center((start.x + end.x) / 2.0f, (start.y + end.y) / 2.0f);

                    newShape = std::make_unique<EllipseShape>(
                        Ellipse2D(center, rx, ry),
                        m_toolManager.GetStrokeColor(),
                        m_toolManager.GetStrokeWidth(),
                        m_toolManager.IsFilled(),
                        m_toolManager.GetFillColor()
                    );
                    break;
                }

                case ToolType::Line:
                    newShape = std::make_unique<LineShape>(
                        start, end,
                        m_toolManager.GetStrokeColor(),
                        m_toolManager.GetStrokeWidth()
                    );
                    break;

                case ToolType::Text:
                {
                    wchar_t szBuffer[256] = L"Direct2D Text";
                    newShape = std::make_unique<TextShape>(
                        szBuffer, start,
                        L"맑은 고딕", 20.0f,
                        m_toolManager.GetStrokeColor()
                    );
                    break;
                }

                default:
                    break;
                }

                if (newShape)
                {
                    IShape* pRaw = newShape.get();
                    m_commandHistory.ExecuteCommand(std::make_unique<AddShapeCommand>(
                        m_document, std::move(newShape)
                    ));
                    // Select newly completed working shape so edit points show immediately
                    m_selectionManager.Select(pRaw);
                }
            }
        }

        m_mouseState.bIsDragging = false;

        if (hWnd && m_bCapturingMouse && GetCapture() == hWnd)
        {
            m_bCapturingMouse = false;
            ReleaseCapture();
        }
        else
        {
            m_bCapturingMouse = false;
        }
    }

    void OnCaptureChanged()
    {
        m_bCapturingMouse = false;
        m_mouseState.bLButtonDown = false;
        m_mouseState.bIsDragging = false;
    }

    void OnCancelMode(HWND hWnd)
    {
        m_polygonPoints.clear();
        m_mouseState.bLButtonDown = false;
        m_mouseState.bIsDragging = false;

        if (hWnd && m_bCapturingMouse && GetCapture() == hWnd)
        {
            m_bCapturingMouse = false;
            ReleaseCapture();
        }
        else
        {
            m_bCapturingMouse = false;
        }
    }

    void OnEscapeKey(HWND hWnd)
    {
        m_polygonPoints.clear();
        if (m_mouseState.bIsDragging)
        {
            m_mouseState.bLButtonDown = false;
            m_mouseState.bIsDragging = false;

            if (hWnd && m_bCapturingMouse && GetCapture() == hWnd)
            {
                m_bCapturingMouse = false;
                ReleaseCapture();
            }
            else
            {
                m_bCapturingMouse = false;
            }
        }
    }

    void OnRButtonDown(Point2D pt)
    {
        m_mouseState.currentPos = pt;
        m_mouseState.bRButtonDown = true;
    }

    void OnRButtonUp(Point2D pt)
    {
        m_mouseState.currentPos = pt;
        m_mouseState.bRButtonDown = false;
    }

    void OnMButtonDown(Point2D pt)
    {
        m_mouseState.currentPos = pt;
        m_mouseState.bMButtonDown = true;
    }

    void OnMButtonUp(Point2D pt)
    {
        m_mouseState.currentPos = pt;
        m_mouseState.bMButtonDown = false;
    }

    void OnMouseLeave()
    {
        m_mouseState.bInCanvas = false;
    }
};

#endif // EDITOR_CONTROLLER_H
