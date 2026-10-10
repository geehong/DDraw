#ifndef DOCKING_MANAGER_H
#define DOCKING_MANAGER_H

#include "DockPanel.h"
#include <vector>
#include <strsafe.h>

class DockingManager
{
private:
    std::vector<DockPanel*> m_panels;

public:
    DockingManager() {}

    void AddPanel(DockPanel* pPanel)
    {
        if (pPanel)
        {
            m_panels.push_back(pPanel);
        }
    }

    void LayoutPanels(int clientWidth, int clientHeight, int topOffset, int bottomOffset, HWND hCanvas)
    {
        int availableX = 0;
        int availableY = topOffset;
        int availableW = clientWidth;
        int availableH = clientHeight - topOffset - bottomOffset;
        if (availableH < 0) availableH = 0;

        wchar_t dbgBuf[256];
        StringCchPrintf(dbgBuf, 256, L"[DockingManager] LayoutPanels: clientW=%d, clientH=%d, top=%d, bot=%d, availableW=%d, availableH=%d\n",
            clientWidth, clientHeight, topOffset, bottomOffset, availableW, availableH);
        OutputDebugString(dbgBuf);

        for (auto* pPanel : m_panels)
        {
            if (!pPanel || !pPanel->GetHWND()) continue;

            int panelSize = pPanel->GetPanelSize();
            DockPosition pos = pPanel->GetDockPosition();
            const wchar_t* posStr = (pos == DockPosition::Left) ? L"Left" :
                                    (pos == DockPosition::Right) ? L"Right" :
                                    (pos == DockPosition::Top) ? L"Top" : L"Bottom";

            switch (pos)
            {
            case DockPosition::Left:
            {
                int w = (panelSize > availableW) ? availableW : panelSize;
                MoveWindow(pPanel->GetHWND(), availableX, availableY, w, availableH, TRUE);
                StringCchPrintf(dbgBuf, 256, L"  Panel '%s' [%s]: X=%d, Y=%d, W=%d, H=%d\n", pPanel->GetTitle().c_str(), posStr, availableX, availableY, w, availableH);
                OutputDebugString(dbgBuf);
                availableX += w;
                availableW -= w;
                break;
            }
            case DockPosition::Right:
            {
                int w = (panelSize > availableW) ? availableW : panelSize;
                MoveWindow(pPanel->GetHWND(), availableX + availableW - w, availableY, w, availableH, TRUE);
                StringCchPrintf(dbgBuf, 256, L"  Panel '%s' [%s]: X=%d, Y=%d, W=%d, H=%d\n", pPanel->GetTitle().c_str(), posStr, availableX + availableW - w, availableY, w, availableH);
                OutputDebugString(dbgBuf);
                availableW -= w;
                break;
            }
            case DockPosition::Top:
            {
                int h = 64;
                if (h > availableH) h = availableH;
                MoveWindow(pPanel->GetHWND(), availableX, availableY, availableW, h, TRUE);
                StringCchPrintf(dbgBuf, 256, L"  Panel '%s' [%s]: X=%d, Y=%d, W=%d, H=%d\n", pPanel->GetTitle().c_str(), posStr, availableX, availableY, availableW, h);
                OutputDebugString(dbgBuf);
                availableY += h;
                availableH -= h;
                break;
            }
            case DockPosition::Bottom:
            {
                int h = 64;
                if (h > availableH) h = availableH;
                MoveWindow(pPanel->GetHWND(), availableX, availableY + availableH - h, availableW, h, TRUE);
                StringCchPrintf(dbgBuf, 256, L"  Panel '%s' [%s]: X=%d, Y=%d, W=%d, H=%d\n", pPanel->GetTitle().c_str(), posStr, availableX, availableY + availableH - h, availableW, h);
                OutputDebugString(dbgBuf);
                availableH -= h;
                break;
            }
            }
        }

        if (hCanvas)
        {
            if (availableW < 0) availableW = 0;
            if (availableH < 0) availableH = 0;
            MoveWindow(hCanvas, availableX, availableY, availableW, availableH, TRUE);
            StringCchPrintf(dbgBuf, 256, L"  Canvas: X=%d, Y=%d, W=%d, H=%d\n", availableX, availableY, availableW, availableH);
            OutputDebugString(dbgBuf);
        }
    }
};

#endif // DOCKING_MANAGER_H
