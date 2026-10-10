#ifndef DOCK_PREVIEW_WINDOW_H
#define DOCK_PREVIEW_WINDOW_H

#include "../BaseWindow.h"

class DockPreviewWindow : public BaseWindow
{
public:
    DockPreviewWindow() {}

    void ShowPreview(HWND hParent, RECT screenRect)
    {
        if (!m_hWnd)
        {
            Create(
                L"DockPreview",
                WS_POPUP,
                WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                screenRect.left, screenRect.top,
                screenRect.right - screenRect.left, screenRect.bottom - screenRect.top,
                hParent, NULL
            );
            SetLayeredWindowAttributes(m_hWnd, 0, 120, LWA_ALPHA);
        }
        else
        {
            SetWindowPos(
                m_hWnd, HWND_TOPMOST,
                screenRect.left, screenRect.top,
                screenRect.right - screenRect.left, screenRect.bottom - screenRect.top,
                SWP_SHOWWINDOW | SWP_NOACTIVATE
            );
        }
        InvalidateRect(m_hWnd, NULL, FALSE);
    }

    void HidePreview()
    {
        if (m_hWnd)
        {
            ShowWindow(m_hWnd, SW_HIDE);
        }
    }

protected:
    virtual PCWSTR GetClassName() const override { return L"DockPreviewWindowClass"; }

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

            HBRUSH hBrush = CreateSolidBrush(RGB(51, 153, 255));
            FillRect(hdc, &crt, hBrush);
            DeleteObject(hBrush);

            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 102, 204));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, crt.left, crt.top, crt.right, crt.bottom);
            SelectObject(hdc, hOldBrush);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);

            EndPaint(m_hWnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        }
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
};

#endif // DOCK_PREVIEW_WINDOW_H
