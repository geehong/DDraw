#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class BaseWindow
{
protected:
    HWND m_hWnd;

public:
    BaseWindow() : m_hWnd(NULL) {}
    virtual ~BaseWindow() {}

    HWND GetHWND() const { return m_hWnd; }

    BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = NULL,
        HMENU hMenu = NULL
    )
    {
        WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
        wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.lpfnWndProc   = BaseWindow::WindowProc;
        wc.hInstance     = GetModuleHandle(NULL);
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = GetClassName();

        RegisterClassEx(&wc);

        m_hWnd = CreateWindowEx(
            dwExStyle,
            GetClassName(),
            lpWindowName,
            dwStyle,
            x, y, nWidth, nHeight,
            hWndParent,
            hMenu,
            GetModuleHandle(NULL),
            this
        );

        return (m_hWnd != NULL);
    }

protected:
    virtual PCWSTR GetClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        BaseWindow* pSelf = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pSelf = reinterpret_cast<BaseWindow*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pSelf));
            pSelf->m_hWnd = hWnd;
        }
        else
        {
            pSelf = reinterpret_cast<BaseWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (pSelf)
        {
            return pSelf->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
};

#endif // BASE_WINDOW_H
