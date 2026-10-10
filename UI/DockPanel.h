#ifndef DOCK_PANEL_H
#define DOCK_PANEL_H

#include "../BaseWindow.h"
#include "DockPreviewWindow.h"
#include <string>
#include <strsafe.h>

enum class DockPosition
{
    Left,
    Right,
    Top,
    Bottom
};

class DockPanel : public BaseWindow
{
private:
    std::wstring m_title;
    DockPosition m_dockPosition;
    int m_size;
    bool m_bDraggingHeader;
    POINT m_dragStart;
    DockPreviewWindow m_previewWindow;

public:
    DockPanel(const wchar_t* title, DockPosition pos, int defaultSize = 160)
        : m_title(title), m_dockPosition(pos), m_size(defaultSize),
          m_bDraggingHeader(false) {}

    DockPosition GetDockPosition() const { return m_dockPosition; }
    void SetDockPosition(DockPosition pos) { m_dockPosition = pos; }

    int GetPanelSize() const { return m_size; }
    void SetPanelSize(int size) { m_size = size; }

    const std::wstring& GetTitle() const { return m_title; }

protected:
    virtual PCWSTR GetClassName() const override { return L"DockPanelClass"; }

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    {
        switch (uMsg)
        {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hWnd, &ps);
            RECT crt;
            GetClientRect(m_hWnd, &crt);

            // Header background (Top 26px)
            RECT hdrRect = { crt.left, crt.top, crt.right, crt.top + 26 };
            HBRUSH hHdrBrush = CreateSolidBrush(RGB(45, 55, 72)); // Modern dark slate
            FillRect(hdc, &hdrRect, hHdrBrush);
            DeleteObject(hHdrBrush);

            // Header top highlight line (accent blue line)
            RECT topLine = { hdrRect.left, hdrRect.top, hdrRect.right, hdrRect.top + 2 };
            HBRUSH hAccentBrush = CreateSolidBrush(RGB(66, 153, 225));
            FillRect(hdc, &topLine, hAccentBrush);
            DeleteObject(hAccentBrush);

            // Header bottom border line
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(30, 40, 55));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            MoveToEx(hdc, hdrRect.left, hdrRect.bottom - 1, NULL);
            LineTo(hdc, hdrRect.right, hdrRect.bottom - 1);

            // Grip dots on the left of header title
            for (int i = 0; i < 3; ++i)
            {
                RECT gripRect = { hdrRect.left + 6 + (i * 4), hdrRect.top + 10, hdrRect.left + 8 + (i * 4), hdrRect.top + 16 };
                HBRUSH hGripBrush = CreateSolidBrush(RGB(160, 174, 192));
                FillRect(hdc, &gripRect, hGripBrush);
                DeleteObject(hGripBrush);
            }

            // Header title text (bold white/light gray text)
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(240, 245, 250));

            LOGFONT lf = { 0 };
            lf.lfHeight = -12;
            lf.lfWeight = FW_BOLD;
            wcscpy_s(lf.lfFaceName, L"맑은 고딕");
            HFONT hFont = CreateFontIndirect(&lf);
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            RECT textRect = { hdrRect.left + 22, hdrRect.top + 2, hdrRect.right - 8, hdrRect.bottom };
            DrawText(hdc, m_title.c_str(), -1, &textRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);

            // Content area background (Light slate tint matching modern panel controls)
            RECT bodyRect = { crt.left, crt.top + 26, crt.right, crt.bottom };
            HBRUSH hBodyBrush = CreateSolidBrush(RGB(240, 243, 248));
            FillRect(hdc, &bodyRect, hBodyBrush);
            DeleteObject(hBodyBrush);

            // Outer frame
            HPEN hFramePen = CreatePen(PS_SOLID, 1, RGB(200, 205, 215));
            HPEN hOldFramePen = (HPEN)SelectObject(hdc, hFramePen);
            HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
            HBRUSH hOldNull = (HBRUSH)SelectObject(hdc, hNullBrush);
            Rectangle(hdc, crt.left, crt.top, crt.right, crt.bottom);
            SelectObject(hdc, hOldNull);
            SelectObject(hdc, hOldFramePen);
            DeleteObject(hFramePen);

            EndPaint(m_hWnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            int y = HIWORD(lParam);
            if (y <= 26)
            {
                m_bDraggingHeader = true;
                GetCursorPos(&m_dragStart);
                SetCapture(m_hWnd);
            }
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (m_bDraggingHeader)
            {
                POINT pt;
                GetCursorPos(&pt);
                HWND hParent = GetParent(m_hWnd);
                if (hParent)
                {
                    RECT parentClientRect;
                    GetClientRect(hParent, &parentClientRect);
                    POINT parentPt = pt;
                    ScreenToClient(hParent, &parentPt);

                    RECT parentScreenRect = parentClientRect;
                    POINT topLeft = { parentClientRect.left, parentClientRect.top };
                    POINT bottomRight = { parentClientRect.right, parentClientRect.bottom };
                    ClientToScreen(hParent, &topLeft);
                    ClientToScreen(hParent, &bottomRight);
                    parentScreenRect.left = topLeft.x;
                    parentScreenRect.top = topLeft.y;
                    parentScreenRect.right = bottomRight.x;
                    parentScreenRect.bottom = bottomRight.y;

                    int marginX = parentClientRect.right / 4;
                    int marginY = parentClientRect.bottom / 4;

                    RECT previewRect = parentScreenRect;
                    if (parentPt.x < marginX)
                    {
                        previewRect.right = previewRect.left + m_size;
                    }
                    else if (parentPt.x > parentClientRect.right - marginX)
                    {
                        previewRect.left = previewRect.right - m_size;
                    }
                    else if (parentPt.y < marginY)
                    {
                        previewRect.bottom = previewRect.top + m_size;
                    }
                    else if (parentPt.y > parentClientRect.bottom - marginY)
                    {
                        previewRect.top = previewRect.bottom - m_size;
                    }

                    m_previewWindow.ShowPreview(hParent, previewRect);
                }
            }
            return 0;
        }

        case WM_LBUTTONUP:
        {
            if (m_bDraggingHeader)
            {
                m_bDraggingHeader = false;
                ReleaseCapture();
                m_previewWindow.HidePreview();

                POINT pt;
                GetCursorPos(&pt);
                HWND hParent = GetParent(m_hWnd);
                if (hParent)
                {
                    RECT parentRect;
                    GetClientRect(hParent, &parentRect);
                    POINT parentPt = pt;
                    ScreenToClient(hParent, &parentPt);

                    int marginX = parentRect.right / 4;
                    int marginY = parentRect.bottom / 4;

                    DockPosition oldPos = m_dockPosition;
                    if (parentPt.x < marginX)
                    {
                        m_dockPosition = DockPosition::Left;
                    }
                    else if (parentPt.x > parentRect.right - marginX)
                    {
                        m_dockPosition = DockPosition::Right;
                    }
                    else if (parentPt.y < marginY)
                    {
                        m_dockPosition = DockPosition::Top;
                    }
                    else if (parentPt.y > parentRect.bottom - marginY)
                    {
                        m_dockPosition = DockPosition::Bottom;
                    }

                    wchar_t dbgBuf[256];
                    StringCchPrintf(dbgBuf, 256, L"[DockPanel] Drag end: Panel '%s' Pos changed (%d -> %d), parentPt=(%d,%d)\n",
                        m_title.c_str(), (int)oldPos, (int)m_dockPosition, parentPt.x, parentPt.y);
                    OutputDebugString(dbgBuf);

                    SendMessage(hParent, WM_SIZE, 0, MAKELPARAM(parentRect.right, parentRect.bottom));
                }
            }
            return 0;
        }

        case WM_HSCROLL:
        case WM_VSCROLL:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
        case WM_COMMAND:
        {
            HWND hParent = GetParent(m_hWnd);
            if (hParent)
            {
                return SendMessage(hParent, uMsg, wParam, lParam);
            }
            break;
        }

        case WM_ERASEBKGND:
            return 1;
        }
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
};

#endif // DOCK_PANEL_H
