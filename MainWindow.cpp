#include "MainWindow.h"
#include "resource.h"
#include "Serialization/DocumentSerializer.h"
#include <commdlg.h>
#include <strsafe.h>

#pragma comment(lib, "comctl32.lib")

MainWindow::MainWindow()
    : m_canvasWindow(m_toolManager),
      m_leftDockPanel(L" Tools Panel ", DockPosition::Left, 140),
      m_rightDockPanel(L" MCP Properties ", DockPosition::Right, 180),
      m_hToolBar(NULL),
      m_hStatusBar(NULL),
      m_hLeftPanel(NULL),
      m_hRightMCP(NULL),
      m_bIsDirty(false),
      m_currentFileName(L"Untitled"),
      m_currentFilePath(L"")
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::UpdateTitleBar()
{
    bool isCmdDirty = m_canvasWindow.GetCommandHistory().IsDirty();
    m_bIsDirty = isCmdDirty;

    std::wstring title = L"D2Draw02 - " + m_currentFileName;
    if (m_bIsDirty)
    {
        title += L" *";
    }
    SetWindowText(m_hWnd, title.c_str());
}

void MainWindow::UpdateStatusBar(float x, float y, const wchar_t* statusText)
{
    if (!m_hStatusBar) return;
    
    wchar_t buf[256];
    if (statusText)
    {
        StringCchPrintf(buf, 256, L"X: %.1f, Y: %.1f | %s", x, y, statusText);
    }
    else
    {
        StringCchPrintf(buf, 256, L"X: %.1f, Y: %.1f", x, y);
    }
    SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)buf);
}

void MainWindow::CreateAppMenu()
{
    HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
    if (hMenu)
    {
        SetMenu(m_hWnd, hMenu);
    }
}

void MainWindow::CreateAppToolBar()
{
    TBBUTTON toolButtons[] = {
        {0, IDM_SHAPE_SELECT,    TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0},
        {0, 0,                   0,               TBSTYLE_SEP,        0, 0},
        {1, IDM_SHAPE_LINE,      TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0},
        {2, IDM_SHAPE_ELLIPSE,   TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0},
        {3, IDM_SHAPE_RECTANGLE, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0},
        {4, IDM_SHAPE_TEXT,      TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0}
    };

    m_hToolBar = CreateToolbarEx(
        m_hWnd,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
        1, 5, GetModuleHandle(NULL), IDB_TOOLBAR,
        toolButtons, 6,
        16, 15, 16, 15, sizeof(TBBUTTON)
    );

    if (m_hToolBar)
    {
        SendMessage(m_hToolBar, TB_CHECKBUTTON, IDM_SHAPE_SELECT, TRUE);
    }
}

void MainWindow::CreateAppStatusBar()
{
    m_hStatusBar = CreateStatusWindow(
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        L"Ready",
        m_hWnd,
        2
    );
}

#define IDC_STROKE_COLOR_BTN       1050
#define IDC_FILL_COLOR_BTN         1051
#define IDC_STROKE_WIDTH_COMBO     1052
#define IDC_FILLED_CHECK           1053

#define IDC_GRID_UNIT_COMBO        1060
#define IDC_SHOW_GRID_CHECK        1061

#define IDC_MAIN_GRID_STEP_COMBO   1062
#define IDC_MAIN_LINE_WIDTH_TRACK  1064
#define IDC_MAIN_GRID_COLOR_BTN    1067
#define IDC_MAIN_OPACITY_TRACK     1068

#define IDC_SUB_DIVISIONS_COMBO    1063
#define IDC_SUB_LINE_WIDTH_TRACK   1065
#define IDC_SUB_GRID_COLOR_BTN     1069
#define IDC_SUB_OPACITY_TRACK      1070

#define IDC_SNAP_TO_GRID_CHECK     1066

void MainWindow::CreateSoftimagePanels()
{
    HINSTANCE hInst = GetModuleHandle(NULL);

    m_leftDockPanel.Create(L"Tools", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 140, 0, m_hWnd, NULL);
    m_rightDockPanel.Create(L"MCP Properties", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 180, 0, m_hWnd, NULL);

    m_hLeftPanel = m_leftDockPanel.GetHWND();
    m_hRightMCP = m_rightDockPanel.GetHWND();

    m_dockingManager.AddPanel(&m_leftDockPanel);
    m_dockingManager.AddPanel(&m_rightDockPanel);

    // Left Panel Children Controls
    m_hLblToolsHeader = CreateWindowEx(0, L"STATIC", L" [ Tools ]", WS_CHILD | WS_VISIBLE | SS_CENTER,
        5, 28, 130, 18, m_hLeftPanel, NULL, hInst, NULL);

    m_hBtnSelect = CreateWindowEx(0, L"BUTTON", L"Select", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
        10, 50, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_SHAPE_SELECT, hInst, NULL);

    m_hBtnRect = CreateWindowEx(0, L"BUTTON", L"Rectangle", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
        10, 80, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_SHAPE_RECTANGLE, hInst, NULL);

    m_hBtnEllipse = CreateWindowEx(0, L"BUTTON", L"Ellipse", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
        10, 110, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_SHAPE_ELLIPSE, hInst, NULL);

    m_hBtnLine = CreateWindowEx(0, L"BUTTON", L"Line", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
        10, 140, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_SHAPE_LINE, hInst, NULL);

    m_hBtnText = CreateWindowEx(0, L"BUTTON", L"Text", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
        10, 170, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_SHAPE_TEXT, hInst, NULL);

    m_hBtnPolygon = CreateWindowEx(0, L"BUTTON", L"Polygon", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
        10, 200, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_SHAPE_POLYGON, hInst, NULL);

    m_hLblActionsHeader = CreateWindowEx(0, L"STATIC", L" [ Actions ]", WS_CHILD | WS_VISIBLE | SS_CENTER,
        5, 238, 130, 18, m_hLeftPanel, NULL, hInst, NULL);

    m_hBtnUndo = CreateWindowEx(0, L"BUTTON", L"Undo", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 260, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_EDIT_UNDO, hInst, NULL);

    m_hBtnRedo = CreateWindowEx(0, L"BUTTON", L"Redo", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 290, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_EDIT_REDO, hInst, NULL);

    m_hBtnDelete = CreateWindowEx(0, L"BUTTON", L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 320, 120, 26, m_hLeftPanel, (HMENU)(UINT_PTR)IDM_EDIT_DELETE, hInst, NULL);

    // Right Panel Children Controls - Style & Grid Settings (Grouped with GroupBoxes)
    m_hGrpShapeProperties = CreateWindowEx(0, L"BUTTON", L" MCP Properties ",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        5, 28, 170, 180, m_hRightMCP, NULL, hInst, NULL);

    m_hLblLineWidth = CreateWindowEx(0, L"STATIC", L"Line Width:", WS_CHILD | WS_VISIBLE,
        15, 48, 150, 18, m_hRightMCP, NULL, hInst, NULL);

    m_hComboStrokeWidth = CreateWindowEx(0, L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        15, 68, 150, 150, m_hRightMCP, (HMENU)(UINT_PTR)IDC_STROKE_WIDTH_COMBO, hInst, NULL);

    const wchar_t* widths[] = { L"1.0 px", L"2.0 px", L"3.0 px", L"5.0 px", L"8.0 px", L"12.0 px" };
    for (int i = 0; i < 6; ++i) SendMessage(m_hComboStrokeWidth, CB_ADDSTRING, 0, (LPARAM)widths[i]);
    SendMessage(m_hComboStrokeWidth, CB_SETCURSEL, 1, 0);

    m_hBtnStrokeColor = CreateWindowEx(0, L"BUTTON", L"Stroke Color...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        15, 100, 150, 26, m_hRightMCP, (HMENU)(UINT_PTR)IDC_STROKE_COLOR_BTN, hInst, NULL);

    m_hBtnFillColor = CreateWindowEx(0, L"BUTTON", L"Fill Color...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        15, 132, 150, 26, m_hRightMCP, (HMENU)(UINT_PTR)IDC_FILL_COLOR_BTN, hInst, NULL);

    m_hChkFilled = CreateWindowEx(0, L"BUTTON", L"Filled Shape", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        15, 164, 150, 22, m_hRightMCP, (HMENU)(UINT_PTR)IDC_FILLED_CHECK, hInst, NULL);

    // --- Grid Settings Outer GroupBox ---
    m_hGrpGridSettings = CreateWindowEx(0, L"BUTTON", L" Grid Settings ",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        5, 215, 170, 560, m_hRightMCP, NULL, hInst, NULL);

    m_hLblGridUnit = CreateWindowEx(0, L"STATIC", L"Unit:", WS_CHILD | WS_VISIBLE,
        15, 238, 150, 18, m_hRightMCP, NULL, hInst, NULL);

    m_hComboGridUnit = CreateWindowEx(0, L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        15, 256, 150, 150, m_hRightMCP, (HMENU)(UINT_PTR)IDC_GRID_UNIT_COMBO, hInst, NULL);

    const wchar_t* gridUnits[] = { L"Point (pt)", L"Millimeter (mm)", L"Centimeter (cm)", L"Meter (m)", L"Inch (in)" };
    for (int i = 0; i < 5; ++i) SendMessage(m_hComboGridUnit, CB_ADDSTRING, 0, (LPARAM)gridUnits[i]);
    SendMessage(m_hComboGridUnit, CB_SETCURSEL, 0, 0); // Default Point

    m_hChkShowGrid = CreateWindowEx(0, L"BUTTON", L"Show Grid Lines", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        15, 288, 150, 22, m_hRightMCP, (HMENU)(UINT_PTR)IDC_SHOW_GRID_CHECK, hInst, NULL);
    SendMessage(m_hChkShowGrid, BM_SETCHECK, BST_CHECKED, 0);

    // =========================================================
    // [ Main Grid GroupBox ] (Step, Width Trackbar, Color, Opacity Trackbar)
    // =========================================================
    m_hGrpMainGrid = CreateWindowEx(0, L"BUTTON", L" Main Grid ",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        12, 316, 156, 210, m_hRightMCP, NULL, hInst, NULL);

    m_hLblMainGridStep = CreateWindowEx(0, L"STATIC", L"Step:", WS_CHILD | WS_VISIBLE,
        20, 336, 140, 16, m_hRightMCP, NULL, hInst, NULL);
    m_hComboMainGridStep = CreateWindowEx(0, L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        20, 354, 140, 150, m_hRightMCP, (HMENU)(UINT_PTR)IDC_MAIN_GRID_STEP_COMBO, hInst, NULL);
    const wchar_t* mainSteps[] = { L"20 unit", L"50 unit (50P)", L"100 unit", L"200 unit" };
    for (int i = 0; i < 4; ++i) SendMessage(m_hComboMainGridStep, CB_ADDSTRING, 0, (LPARAM)mainSteps[i]);
    SendMessage(m_hComboMainGridStep, CB_SETCURSEL, 1, 0); // 50 unit default

    m_hLblMainLineWidth = CreateWindowEx(0, L"STATIC", L"Width: 1.0 px", WS_CHILD | WS_VISIBLE,
        20, 384, 140, 16, m_hRightMCP, NULL, hInst, NULL);
    m_hTrackMainLineWidth = CreateWindowEx(0, TRACKBAR_CLASS, L"",
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS,
        20, 400, 140, 26, m_hRightMCP, (HMENU)(UINT_PTR)IDC_MAIN_LINE_WIDTH_TRACK, hInst, NULL);
    SendMessage(m_hTrackMainLineWidth, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30)); // 0.1px ~ 3.0px (1~30)
    SendMessage(m_hTrackMainLineWidth, TBM_SETPOS, TRUE, 10); // 1.0 px default

    m_hBtnMainGridColor = CreateWindowEx(0, L"BUTTON", L"Color...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, 430, 140, 24, m_hRightMCP, (HMENU)(UINT_PTR)IDC_MAIN_GRID_COLOR_BTN, hInst, NULL);

    m_hLblMainOpacity = CreateWindowEx(0, L"STATIC", L"Opacity: 95%", WS_CHILD | WS_VISIBLE,
        20, 458, 140, 16, m_hRightMCP, NULL, hInst, NULL);
    m_hTrackMainOpacity = CreateWindowEx(0, TRACKBAR_CLASS, L"",
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS,
        20, 474, 140, 26, m_hRightMCP, (HMENU)(UINT_PTR)IDC_MAIN_OPACITY_TRACK, hInst, NULL);
    SendMessage(m_hTrackMainOpacity, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100)); // 0% ~ 100%
    SendMessage(m_hTrackMainOpacity, TBM_SETPOS, TRUE, 95); // 95% default

    // =========================================================
    // [ Sub Grid GroupBox ] (Split, Width Trackbar, Color, Opacity Trackbar)
    // =========================================================
    m_hGrpSubGrid = CreateWindowEx(0, L"BUTTON", L" Sub Grid ",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        12, 532, 156, 210, m_hRightMCP, NULL, hInst, NULL);

    m_hLblSubDivisions = CreateWindowEx(0, L"STATIC", L"Split Count:", WS_CHILD | WS_VISIBLE,
        20, 552, 140, 16, m_hRightMCP, NULL, hInst, NULL);
    m_hComboSubDivisions = CreateWindowEx(0, L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        20, 570, 140, 150, m_hRightMCP, (HMENU)(UINT_PTR)IDC_SUB_DIVISIONS_COMBO, hInst, NULL);
    const wchar_t* subSplits[] = { L"2 splits", L"5 splits (10P sub)", L"10 splits (5P sub)", L"20 splits" };
    for (int i = 0; i < 4; ++i) SendMessage(m_hComboSubDivisions, CB_ADDSTRING, 0, (LPARAM)subSplits[i]);
    SendMessage(m_hComboSubDivisions, CB_SETCURSEL, 1, 0); // 5 splits default

    m_hLblSubLineWidth = CreateWindowEx(0, L"STATIC", L"Width: 0.5 px", WS_CHILD | WS_VISIBLE,
        20, 600, 140, 16, m_hRightMCP, NULL, hInst, NULL);
    m_hTrackSubLineWidth = CreateWindowEx(0, TRACKBAR_CLASS, L"",
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS,
        20, 616, 140, 26, m_hRightMCP, (HMENU)(UINT_PTR)IDC_SUB_LINE_WIDTH_TRACK, hInst, NULL);
    SendMessage(m_hTrackSubLineWidth, TBM_SETRANGE, TRUE, MAKELPARAM(1, 20)); // 0.1px ~ 2.0px (1~20)
    SendMessage(m_hTrackSubLineWidth, TBM_SETPOS, TRUE, 5); // 0.5 px default

    m_hBtnSubGridColor = CreateWindowEx(0, L"BUTTON", L"Color...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, 646, 140, 24, m_hRightMCP, (HMENU)(UINT_PTR)IDC_SUB_GRID_COLOR_BTN, hInst, NULL);

    m_hLblSubOpacity = CreateWindowEx(0, L"STATIC", L"Opacity: 50%", WS_CHILD | WS_VISIBLE,
        20, 674, 140, 16, m_hRightMCP, NULL, hInst, NULL);
    m_hTrackSubOpacity = CreateWindowEx(0, TRACKBAR_CLASS, L"",
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS,
        20, 690, 140, 26, m_hRightMCP, (HMENU)(UINT_PTR)IDC_SUB_OPACITY_TRACK, hInst, NULL);
    SendMessage(m_hTrackSubOpacity, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100)); // 0% ~ 100%
    SendMessage(m_hTrackSubOpacity, TBM_SETPOS, TRUE, 50); // 50% default

    // Snap to Sub Grid Checkbox
    m_hChkSnapToGrid = CreateWindowEx(0, L"BUTTON", L"Snap to Sub Grid", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        15, 748, 150, 22, m_hRightMCP, (HMENU)(UINT_PTR)IDC_SNAP_TO_GRID_CHECK, hInst, NULL);
    SendMessage(m_hChkSnapToGrid, BM_SETCHECK, BST_CHECKED, 0);

    m_hLblSelectionInfo = CreateWindowEx(0, L"STATIC", L"Status: Ready",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        10, 780, 160, 40, m_hRightMCP, NULL, hInst, NULL);

    LayoutPanelControls();
}

void MainWindow::LayoutPanelControls()
{
    // 1. Left Panel (Tools Panel)
    if (m_hLeftPanel)
    {
        RECT rc;
        GetClientRect(m_hLeftPanel, &rc);
        int w = rc.right - rc.left;
        DockPosition pos = m_leftDockPanel.GetDockPosition();

        if (pos == DockPosition::Top || pos == DockPosition::Bottom)
        {
            // Horizontal layout for Top/Bottom docking
            int headerH = 26;
            int marginY = headerH + 6;
            int btnW = 90;
            int btnH = 26;
            int gap = 6;
            int x = 10;

            // Hide section header labels in top/bottom mode to save vertical space
            if (m_hLblToolsHeader) MoveWindow(m_hLblToolsHeader, 0, 0, 0, 0, TRUE);
            if (m_hLblActionsHeader) MoveWindow(m_hLblActionsHeader, 0, 0, 0, 0, TRUE);

            HWND toolBtns[] = { m_hBtnSelect, m_hBtnRect, m_hBtnEllipse, m_hBtnLine, m_hBtnText, m_hBtnPolygon, m_hBtnUndo, m_hBtnRedo, m_hBtnDelete };
            for (HWND hBtn : toolBtns)
            {
                if (hBtn)
                {
                    MoveWindow(hBtn, x, marginY, btnW, btnH, TRUE);
                    x += btnW + gap;
                }
            }
        }
        else
        {
            // Vertical layout for Left/Right docking
            int headerH = 26;
            int itemW = (w > 20) ? (w - 20) : 120;
            int x = (w - itemW) / 2;
            if (x < 5) x = 5;

            MoveWindow(m_hLblToolsHeader, x, headerH + 6, itemW, 18, TRUE);
            MoveWindow(m_hBtnSelect,     x, headerH + 28, itemW, 26, TRUE);
            MoveWindow(m_hBtnRect,       x, headerH + 58, itemW, 26, TRUE);
            MoveWindow(m_hBtnEllipse,    x, headerH + 88, itemW, 26, TRUE);
            MoveWindow(m_hBtnLine,       x, headerH + 118, itemW, 26, TRUE);
            MoveWindow(m_hBtnText,       x, headerH + 148, itemW, 26, TRUE);
            MoveWindow(m_hBtnPolygon,    x, headerH + 178, itemW, 26, TRUE);

            MoveWindow(m_hLblActionsHeader, x, headerH + 214, itemW, 18, TRUE);
            MoveWindow(m_hBtnUndo,       x, headerH + 236, itemW, 26, TRUE);
            MoveWindow(m_hBtnRedo,       x, headerH + 266, itemW, 26, TRUE);
            MoveWindow(m_hBtnDelete,     x, headerH + 296, itemW, 26, TRUE);
        }
    }

    // 2. Right Panel (MCP Properties)
    if (m_hRightMCP)
    {
        RECT rc;
        GetClientRect(m_hRightMCP, &rc);
        int w = rc.right - rc.left;
        DockPosition pos = m_rightDockPanel.GetDockPosition();

        if (pos == DockPosition::Top || pos == DockPosition::Bottom)
        {
            // Horizontal layout for Top/Bottom docking
            int headerH = 26;
            int marginY = headerH + 6;
            int gap = 8;
            int x = 10;

            if (m_hGrpShapeProperties) MoveWindow(m_hGrpShapeProperties, 0, 0, 0, 0, TRUE);
            if (m_hGrpGridSettings) MoveWindow(m_hGrpGridSettings, 0, 0, 0, 0, TRUE);
            if (m_hGrpMainGrid) MoveWindow(m_hGrpMainGrid, 0, 0, 0, 0, TRUE);
            if (m_hGrpSubGrid) MoveWindow(m_hGrpSubGrid, 0, 0, 0, 0, TRUE);

            // Style controls
            if (m_hLblLineWidth) { MoveWindow(m_hLblLineWidth, x, marginY + 4, 60, 20, TRUE); x += 62; }
            if (m_hComboStrokeWidth) { MoveWindow(m_hComboStrokeWidth, x, marginY, 70, 150, TRUE); x += 70 + gap; }
            if (m_hBtnStrokeColor) { MoveWindow(m_hBtnStrokeColor, x, marginY, 85, 26, TRUE); x += 85 + gap; }
            if (m_hBtnFillColor) { MoveWindow(m_hBtnFillColor, x, marginY, 80, 26, TRUE); x += 80 + gap; }
            if (m_hChkFilled) { MoveWindow(m_hChkFilled, x, marginY + 2, 75, 24, TRUE); x += 75 + gap; }

            // Grid controls
            if (m_hLblGridUnit) { MoveWindow(m_hLblGridUnit, x, marginY + 4, 28, 20, TRUE); x += 30; }
            if (m_hComboGridUnit) { MoveWindow(m_hComboGridUnit, x, marginY, 90, 150, TRUE); x += 90 + gap; }
            if (m_hChkShowGrid) { MoveWindow(m_hChkShowGrid, x, marginY + 2, 75, 24, TRUE); x += 75 + gap; }
            if (m_hComboMainGridStep) { MoveWindow(m_hComboMainGridStep, x, marginY, 90, 150, TRUE); x += 90 + gap; }
            if (m_hComboSubDivisions) { MoveWindow(m_hComboSubDivisions, x, marginY, 95, 150, TRUE); x += 95 + gap; }
            if (m_hChkSnapToGrid) { MoveWindow(m_hChkSnapToGrid, x, marginY + 2, 95, 24, TRUE); x += 95 + gap; }

            if (m_hLblMainGridStep) MoveWindow(m_hLblMainGridStep, 0, 0, 0, 0, TRUE);
            if (m_hLblMainLineWidth) MoveWindow(m_hLblMainLineWidth, 0, 0, 0, 0, TRUE);
            if (m_hTrackMainLineWidth) MoveWindow(m_hTrackMainLineWidth, 0, 0, 0, 0, TRUE);
            if (m_hBtnMainGridColor) MoveWindow(m_hBtnMainGridColor, 0, 0, 0, 0, TRUE);
            if (m_hLblMainOpacity) MoveWindow(m_hLblMainOpacity, 0, 0, 0, 0, TRUE);
            if (m_hTrackMainOpacity) MoveWindow(m_hTrackMainOpacity, 0, 0, 0, 0, TRUE);

            if (m_hLblSubDivisions) MoveWindow(m_hLblSubDivisions, 0, 0, 0, 0, TRUE);
            if (m_hLblSubLineWidth) MoveWindow(m_hLblSubLineWidth, 0, 0, 0, 0, TRUE);
            if (m_hTrackSubLineWidth) MoveWindow(m_hTrackSubLineWidth, 0, 0, 0, 0, TRUE);
            if (m_hBtnSubGridColor) MoveWindow(m_hBtnSubGridColor, 0, 0, 0, 0, TRUE);
            if (m_hLblSubOpacity) MoveWindow(m_hLblSubOpacity, 0, 0, 0, 0, TRUE);
            if (m_hTrackSubOpacity) MoveWindow(m_hTrackSubOpacity, 0, 0, 0, 0, TRUE);

            if (m_hLblSelectionInfo) MoveWindow(m_hLblSelectionInfo, x, marginY + 4, 100, 20, TRUE);
        }
        else
        {
            // Vertical layout for Left/Right docking
            int headerH = 26;
            int itemW = (w > 20) ? (w - 20) : 160;
            int x = (w - itemW) / 2;
            if (x < 5) x = 5;

            int innerX = x + 8;
            int innerW = itemW - 16;
            if (innerW < 100) innerW = 100;

            int grpMainInnerX = x + 14;
            int grpMainInnerW = itemW - 28;
            if (grpMainInnerW < 80) grpMainInnerW = 80;

            // 1. [ MCP Properties ] GroupBox
            MoveWindow(m_hGrpShapeProperties, x, headerH + 4, itemW, 172, TRUE);
            MoveWindow(m_hLblLineWidth,       innerX, headerH + 24, innerW, 16, TRUE);
            MoveWindow(m_hComboStrokeWidth,   innerX, headerH + 42, innerW, 150, TRUE);
            MoveWindow(m_hBtnStrokeColor,     innerX, headerH + 72, innerW, 24, TRUE);
            MoveWindow(m_hBtnFillColor,       innerX, headerH + 100, innerW, 24, TRUE);
            MoveWindow(m_hChkFilled,          innerX, headerH + 128, innerW, 22, TRUE);

            // 2. [ Grid Settings ] GroupBox
            MoveWindow(m_hGrpGridSettings,    x, headerH + 184, itemW, 560, TRUE);
            MoveWindow(m_hLblGridUnit,        innerX, headerH + 204, innerW, 16, TRUE);
            MoveWindow(m_hComboGridUnit,      innerX, headerH + 222, innerW, 150, TRUE);
            MoveWindow(m_hChkShowGrid,        innerX, headerH + 252, innerW, 22, TRUE);

            // 2a. [ Main Grid ] GroupBox
            MoveWindow(m_hGrpMainGrid,        x + 6, headerH + 278, itemW - 12, 210, TRUE);
            MoveWindow(m_hLblMainGridStep,    grpMainInnerX, headerH + 296, grpMainInnerW, 16, TRUE);
            MoveWindow(m_hComboMainGridStep,  grpMainInnerX, headerH + 314, grpMainInnerW, 150, TRUE);
            MoveWindow(m_hLblMainLineWidth,   grpMainInnerX, headerH + 344, grpMainInnerW, 16, TRUE);
            MoveWindow(m_hTrackMainLineWidth, grpMainInnerX, headerH + 360, grpMainInnerW, 26, TRUE);
            MoveWindow(m_hBtnMainGridColor,   grpMainInnerX, headerH + 390, grpMainInnerW, 24, TRUE);
            MoveWindow(m_hLblMainOpacity,     grpMainInnerX, headerH + 418, grpMainInnerW, 16, TRUE);
            MoveWindow(m_hTrackMainOpacity,   grpMainInnerX, headerH + 434, grpMainInnerW, 26, TRUE);

            // 2b. [ Sub Grid ] GroupBox
            MoveWindow(m_hGrpSubGrid,         x + 6, headerH + 494, itemW - 12, 210, TRUE);
            MoveWindow(m_hLblSubDivisions,    grpMainInnerX, headerH + 512, grpMainInnerW, 16, TRUE);
            MoveWindow(m_hComboSubDivisions,  grpMainInnerX, headerH + 530, grpMainInnerW, 150, TRUE);
            MoveWindow(m_hLblSubLineWidth,    grpMainInnerX, headerH + 560, grpMainInnerW, 16, TRUE);
            MoveWindow(m_hTrackSubLineWidth,  grpMainInnerX, headerH + 576, grpMainInnerW, 26, TRUE);
            MoveWindow(m_hBtnSubGridColor,    grpMainInnerX, headerH + 606, grpMainInnerW, 24, TRUE);
            MoveWindow(m_hLblSubOpacity,      grpMainInnerX, headerH + 634, grpMainInnerW, 16, TRUE);
            MoveWindow(m_hTrackSubOpacity,    grpMainInnerX, headerH + 650, grpMainInnerW, 26, TRUE);

            // Snap and status
            MoveWindow(m_hChkSnapToGrid,      innerX, headerH + 710, innerW, 22, TRUE);
            MoveWindow(m_hLblSelectionInfo,   x, headerH + 750, itemW, 40, TRUE);
        }
    }
}

bool MainWindow::PromptSaveIfDirty()
{
    if (!m_canvasWindow.GetCommandHistory().IsDirty())
    {
        return true;
    }

    int result = MessageBox(
        m_hWnd,
        L"Do you want to save changes?",
        L"D2Draw02 - Save Confirmation",
        MB_YESNOCANCEL | MB_ICONQUESTION
    );

    if (result == IDYES)
    {
        return FileSave();
    }
    else if (result == IDNO)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MainWindow::FileNew()
{
    if (!PromptSaveIfDirty()) return;

    m_canvasWindow.GetDocument().Clear();
    m_canvasWindow.GetCommandHistory().Reset();
    m_currentFileName = L"Untitled";
    m_currentFilePath = L"";
    UpdateTitleBar();
    InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
}

void MainWindow::FileOpen()
{
    if (!PromptSaveIfDirty()) return;

    wchar_t szFile[MAX_PATH] = { 0 };
    OPENFILENAME ofn = { sizeof(OPENFILENAME) };
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = L"D2Draw JSON (*.json)\0*.json\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        Document newDoc;
        if (DocumentSerializer::LoadFromFile(newDoc, szFile))
        {
            m_canvasWindow.GetDocument() = newDoc;
            m_canvasWindow.GetCommandHistory().Reset();
            m_currentFilePath = szFile;
            
            wchar_t* pName = wcsrchr(szFile, L'\\');
            m_currentFileName = pName ? (pName + 1) : szFile;

            UpdateTitleBar();
            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        else
        {
            MessageBox(m_hWnd, L"Failed to load file.", L"Error", MB_OK | MB_ICONERROR);
        }
    }
}

bool MainWindow::FileSave()
{
    if (m_currentFilePath.empty())
    {
        return FileSaveAs();
    }

    if (DocumentSerializer::SaveToFileAtomic(m_currentFilePath, m_canvasWindow.GetDocument()))
    {
        m_canvasWindow.GetCommandHistory().MarkSaved();
        UpdateTitleBar();
        return true;
    }

    MessageBox(m_hWnd, L"Failed to save file.", L"Error", MB_OK | MB_ICONERROR);
    return false;
}

bool MainWindow::FileSaveAs()
{
    wchar_t szFile[MAX_PATH] = { 0 };
    OPENFILENAME ofn = { sizeof(OPENFILENAME) };
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = L"D2Draw JSON (*.json)\0*.json\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"json";

    if (GetSaveFileName(&ofn))
    {
        if (DocumentSerializer::SaveToFileAtomic(szFile, m_canvasWindow.GetDocument()))
        {
            m_currentFilePath = szFile;
            wchar_t* pName = wcsrchr(szFile, L'\\');
            m_currentFileName = pName ? (pName + 1) : szFile;
            m_canvasWindow.GetCommandHistory().MarkSaved();
            UpdateTitleBar();
            return true;
        }
        else
        {
            MessageBox(m_hWnd, L"Failed to save file.", L"Error", MB_OK | MB_ICONERROR);
        }
    }
    return false;
}

void MainWindow::OnCreate()
{
    InitCommonControls();
    CreateAppMenu();
    CreateAppToolBar();
    CreateAppStatusBar();
    CreateSoftimagePanels();

    m_canvasWindow.Create(
        L"CanvasWindow",
        WS_CHILD | WS_VISIBLE,
        0,
        0, 0, 0, 0,
        m_hWnd,
        NULL
    );

    UpdateTitleBar();
    UpdateToolUIState(ToolType::Select);
}

void MainWindow::OnSize(UINT width, UINT height)
{
    if (width == 0 || height == 0)
    {
        RECT crt;
        GetClientRect(m_hWnd, &crt);
        width = crt.right - crt.left;
        height = crt.bottom - crt.top;
    }

    int toolHeight = 0;
    if (m_hToolBar)
    {
        SendMessage(m_hToolBar, TB_AUTOSIZE, 0, 0);
        RECT trt;
        GetWindowRect(m_hToolBar, &trt);
        toolHeight = trt.bottom - trt.top;
    }

    int statusHeight = 0;
    if (m_hStatusBar)
    {
        SendMessage(m_hStatusBar, WM_SIZE, 0, 0);
        RECT srt;
        GetWindowRect(m_hStatusBar, &srt);
        statusHeight = srt.bottom - srt.top;
    }

    m_dockingManager.LayoutPanels(width, height, toolHeight, statusHeight, m_canvasWindow.GetHWND());
    LayoutPanelControls();
}

void MainWindow::UpdateToolUIState(ToolType tool)
{
    m_toolManager.SetCurrentTool(tool);

    WORD activeId = IDM_SHAPE_SELECT;
    switch (tool)
    {
    case ToolType::Select:    activeId = IDM_SHAPE_SELECT; break;
    case ToolType::Rectangle: activeId = IDM_SHAPE_RECTANGLE; break;
    case ToolType::Ellipse:   activeId = IDM_SHAPE_ELLIPSE; break;
    case ToolType::Line:      activeId = IDM_SHAPE_LINE; break;
    case ToolType::Text:      activeId = IDM_SHAPE_TEXT; break;
    case ToolType::Polygon:   activeId = IDM_SHAPE_POLYGON; break;
    }

    if (m_hToolBar)
    {
        SendMessage(m_hToolBar, TB_CHECKBUTTON, IDM_SHAPE_SELECT, activeId == IDM_SHAPE_SELECT);
        SendMessage(m_hToolBar, TB_CHECKBUTTON, IDM_SHAPE_RECTANGLE, activeId == IDM_SHAPE_RECTANGLE);
        SendMessage(m_hToolBar, TB_CHECKBUTTON, IDM_SHAPE_ELLIPSE, activeId == IDM_SHAPE_ELLIPSE);
        SendMessage(m_hToolBar, TB_CHECKBUTTON, IDM_SHAPE_LINE, activeId == IDM_SHAPE_LINE);
        SendMessage(m_hToolBar, TB_CHECKBUTTON, IDM_SHAPE_TEXT, activeId == IDM_SHAPE_TEXT);
    }

    // Toggle sunken (pushed down) visual state for Left Panel tool buttons
    if (m_hBtnSelect)  SendMessage(m_hBtnSelect, BM_SETCHECK, (activeId == IDM_SHAPE_SELECT) ? BST_CHECKED : BST_UNCHECKED, 0);
    if (m_hBtnRect)    SendMessage(m_hBtnRect, BM_SETCHECK, (activeId == IDM_SHAPE_RECTANGLE) ? BST_CHECKED : BST_UNCHECKED, 0);
    if (m_hBtnEllipse) SendMessage(m_hBtnEllipse, BM_SETCHECK, (activeId == IDM_SHAPE_ELLIPSE) ? BST_CHECKED : BST_UNCHECKED, 0);
    if (m_hBtnLine)    SendMessage(m_hBtnLine, BM_SETCHECK, (activeId == IDM_SHAPE_LINE) ? BST_CHECKED : BST_UNCHECKED, 0);
    if (m_hBtnText)    SendMessage(m_hBtnText, BM_SETCHECK, (activeId == IDM_SHAPE_TEXT) ? BST_CHECKED : BST_UNCHECKED, 0);
    if (m_hBtnPolygon) SendMessage(m_hBtnPolygon, BM_SETCHECK, (activeId == IDM_SHAPE_POLYGON) ? BST_CHECKED : BST_UNCHECKED, 0);
}

void MainWindow::ApplyPropertyChanges()
{
    IShape* pSelected = m_canvasWindow.GetEditorController().GetSelectionManager().GetSelectedShape();
    if (pSelected)
    {
        m_canvasWindow.GetCommandHistory().ExecuteCommand(
            std::make_unique<ChangeStyleCommand>(
                pSelected,
                m_toolManager.GetStrokeColor(),
                m_toolManager.GetFillColor(),
                m_toolManager.GetStrokeWidth(),
                m_toolManager.IsFilled()
            )
        );
        InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        UpdateTitleBar();
    }
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
    WORD id = LOWORD(wParam);
    switch (id)
    {
    case IDM_FILE_NEW:
        FileNew();
        break;
    case IDM_FILE_OPEN:
        FileOpen();
        break;
    case IDM_FILE_SAVE:
        FileSave();
        break;
    case IDM_FILE_SAVEAS:
        FileSaveAs();
        break;
    case IDM_FILE_EXIT:
        if (PromptSaveIfDirty())
        {
            PostQuitMessage(0);
        }
        break;
    case IDM_EDIT_UNDO:
        m_canvasWindow.GetCommandHistory().Undo();
        UpdateTitleBar();
        InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        break;
    case IDM_EDIT_REDO:
        m_canvasWindow.GetCommandHistory().Redo();
        UpdateTitleBar();
        InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        break;
    case IDM_EDIT_DELETE:
    {
        IShape* pSelected = m_canvasWindow.GetEditorController().GetSelectionManager().GetSelectedShape();
        if (pSelected)
        {
            m_canvasWindow.GetCommandHistory().ExecuteCommand(
                std::make_unique<DeleteShapeCommand>(
                    m_canvasWindow.GetDocument(), pSelected
                )
            );
            m_canvasWindow.GetEditorController().GetSelectionManager().ClearSelection();
            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
            UpdateTitleBar();
        }
        break;
    }
    case IDM_SHAPE_SELECT:
        UpdateToolUIState(ToolType::Select);
        UpdateStatusBar(0, 0, L"Tool: Select");
        break;
    case IDM_SHAPE_RECTANGLE:
        UpdateToolUIState(ToolType::Rectangle);
        UpdateStatusBar(0, 0, L"Tool: Rectangle");
        break;
    case IDM_SHAPE_ELLIPSE:
        UpdateToolUIState(ToolType::Ellipse);
        UpdateStatusBar(0, 0, L"Tool: Ellipse");
        break;
    case IDM_SHAPE_LINE:
        UpdateToolUIState(ToolType::Line);
        UpdateStatusBar(0, 0, L"Tool: Line");
        break;
    case IDM_SHAPE_TEXT:
        UpdateToolUIState(ToolType::Text);
        UpdateStatusBar(0, 0, L"Tool: Text");
        break;
    case IDM_SHAPE_POLYGON:
        UpdateToolUIState(ToolType::Polygon);
        UpdateStatusBar(0, 0, L"Tool: Polygon (1-Click add point, DblClick/Enter finish)");
        break;

    case IDC_STROKE_COLOR_BTN:
    {
        CHOOSECOLOR cc = { sizeof(CHOOSECOLOR) };
        static COLORREF acrCustClr[16];
        Color curColor = m_toolManager.GetStrokeColor();
        cc.hwndOwner = m_hWnd;
        cc.lpCustColors = (LPDWORD)acrCustClr;
        cc.rgbResult = RGB((BYTE)(curColor.r * 255), (BYTE)(curColor.g * 255), (BYTE)(curColor.b * 255));
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;

        if (ChooseColor(&cc))
        {
            Color newColor(GetRValue(cc.rgbResult) / 255.0f, GetGValue(cc.rgbResult) / 255.0f, GetBValue(cc.rgbResult) / 255.0f, 1.0f);
            m_toolManager.SetStrokeColor(newColor);
            ApplyPropertyChanges();
        }
        break;
    }
    case IDC_FILL_COLOR_BTN:
    {
        CHOOSECOLOR cc = { sizeof(CHOOSECOLOR) };
        static COLORREF acrCustClr[16];
        Color curColor = m_toolManager.GetFillColor();
        cc.hwndOwner = m_hWnd;
        cc.lpCustColors = (LPDWORD)acrCustClr;
        cc.rgbResult = RGB((BYTE)(curColor.r * 255), (BYTE)(curColor.g * 255), (BYTE)(curColor.b * 255));
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;

        if (ChooseColor(&cc))
        {
            Color newColor(GetRValue(cc.rgbResult) / 255.0f, GetGValue(cc.rgbResult) / 255.0f, GetBValue(cc.rgbResult) / 255.0f, 1.0f);
            m_toolManager.SetFillColor(newColor);
            ApplyPropertyChanges();
        }
        break;
    }
    case IDC_STROKE_WIDTH_COMBO:
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int sel = (int)SendMessage(m_hComboStrokeWidth, CB_GETCURSEL, 0, 0);
            float widths[] = { 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 12.0f };
            if (sel >= 0 && sel < 6)
            {
                m_toolManager.SetStrokeWidth(widths[sel]);
                ApplyPropertyChanges();
            }
        }
        break;
    case IDC_FILLED_CHECK:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            bool filled = (SendMessage(m_hChkFilled, BM_GETCHECK, 0, 0) == BST_CHECKED);
            m_toolManager.SetFilled(filled);
            ApplyPropertyChanges();
        }
        break;

    case IDC_GRID_UNIT_COMBO:
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int sel = (int)SendMessage(m_hComboGridUnit, CB_GETCURSEL, 0, 0);
            GridUnit units[] = { GridUnit::Point, GridUnit::MM, GridUnit::CM, GridUnit::M, GridUnit::Inch };
            if (sel >= 0 && sel < 5)
            {
                m_canvasWindow.GetEditorController().GetGridSettings().SetUnit(units[sel]);
                InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
            }
        }
        break;

    case IDC_SHOW_GRID_CHECK:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            bool show = (SendMessage(m_hChkShowGrid, BM_GETCHECK, 0, 0) == BST_CHECKED);
            m_canvasWindow.GetEditorController().GetGridSettings().SetGridVisible(show);
            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        break;

    case IDC_MAIN_GRID_STEP_COMBO:
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int sel = (int)SendMessage(m_hComboMainGridStep, CB_GETCURSEL, 0, 0);
            float steps[] = { 20.0f, 50.0f, 100.0f, 200.0f };
            if (sel >= 0 && sel < 4)
            {
                m_canvasWindow.GetEditorController().GetGridSettings().SetMainGridStepUnit(steps[sel]);
                InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
            }
        }
        break;

    case IDC_SUB_DIVISIONS_COMBO:
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int sel = (int)SendMessage(m_hComboSubDivisions, CB_GETCURSEL, 0, 0);
            int splits[] = { 2, 5, 10, 20 };
            if (sel >= 0 && sel < 4)
            {
                m_canvasWindow.GetEditorController().GetGridSettings().SetSubDivisions(splits[sel]);
                InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
            }
        }
        break;

    case IDC_MAIN_GRID_COLOR_BTN:
    {
        CHOOSECOLOR cc = { sizeof(CHOOSECOLOR) };
        static COLORREF acrCustClr[16];
        Color curColor = m_canvasWindow.GetEditorController().GetGridSettings().GetMainGridColor();
        cc.hwndOwner = m_hWnd;
        cc.lpCustColors = (LPDWORD)acrCustClr;
        cc.rgbResult = RGB((BYTE)(curColor.r * 255), (BYTE)(curColor.g * 255), (BYTE)(curColor.b * 255));
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;

        if (ChooseColor(&cc))
        {
            Color newColor(GetRValue(cc.rgbResult) / 255.0f, GetGValue(cc.rgbResult) / 255.0f, GetBValue(cc.rgbResult) / 255.0f, 1.0f);
            m_canvasWindow.GetEditorController().GetGridSettings().SetMainGridColor(newColor);
            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        break;
    }

    case IDC_SUB_GRID_COLOR_BTN:
    {
        CHOOSECOLOR cc = { sizeof(CHOOSECOLOR) };
        static COLORREF acrCustClr[16];
        Color curColor = m_canvasWindow.GetEditorController().GetGridSettings().GetSubGridColor();
        cc.hwndOwner = m_hWnd;
        cc.lpCustColors = (LPDWORD)acrCustClr;
        cc.rgbResult = RGB((BYTE)(curColor.r * 255), (BYTE)(curColor.g * 255), (BYTE)(curColor.b * 255));
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;

        if (ChooseColor(&cc))
        {
            Color newColor(GetRValue(cc.rgbResult) / 255.0f, GetGValue(cc.rgbResult) / 255.0f, GetBValue(cc.rgbResult) / 255.0f, 1.0f);
            m_canvasWindow.GetEditorController().GetGridSettings().SetSubGridColor(newColor);
            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        break;
    }

    case IDC_SNAP_TO_GRID_CHECK:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            bool snap = (SendMessage(m_hChkSnapToGrid, BM_GETCHECK, 0, 0) == BST_CHECKED);
            m_canvasWindow.GetEditorController().GetGridSettings().SetSnapToGrid(snap);
            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        break;
    }
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate();
        return 0;

    case WM_SIZE:
        OnSize(LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_HSCROLL:
    {
        HWND hTrack = (HWND)lParam;
        if (hTrack == m_hTrackMainLineWidth)
        {
            int pos = (int)SendMessage(m_hTrackMainLineWidth, TBM_GETPOS, 0, 0);
            float width = static_cast<float>(pos) / 10.0f; // 1~30 -> 0.1px ~ 3.0px
            m_canvasWindow.GetEditorController().GetGridSettings().SetMainGridLineWidth(width);

            wchar_t buf[64];
            swprintf_s(buf, L"Width: %.1f px", width);
            SetWindowText(m_hLblMainLineWidth, buf);

            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        else if (hTrack == m_hTrackMainOpacity)
        {
            int pos = (int)SendMessage(m_hTrackMainOpacity, TBM_GETPOS, 0, 0);
            float opacity = static_cast<float>(pos) / 100.0f; // 0~100%
            m_canvasWindow.GetEditorController().GetGridSettings().SetMainGridOpacity(opacity);

            wchar_t buf[64];
            swprintf_s(buf, L"Opacity: %d%%", pos);
            SetWindowText(m_hLblMainOpacity, buf);

            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        else if (hTrack == m_hTrackSubLineWidth)
        {
            int pos = (int)SendMessage(m_hTrackSubLineWidth, TBM_GETPOS, 0, 0);
            float width = static_cast<float>(pos) / 10.0f; // 1~20 -> 0.1px ~ 2.0px
            m_canvasWindow.GetEditorController().GetGridSettings().SetSubGridLineWidth(width);

            wchar_t buf[64];
            swprintf_s(buf, L"Width: %.1f px", width);
            SetWindowText(m_hLblSubLineWidth, buf);

            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        else if (hTrack == m_hTrackSubOpacity)
        {
            int pos = (int)SendMessage(m_hTrackSubOpacity, TBM_GETPOS, 0, 0);
            float opacity = static_cast<float>(pos) / 100.0f; // 0~100%
            m_canvasWindow.GetEditorController().GetGridSettings().SetSubGridOpacity(opacity);

            wchar_t buf[64];
            swprintf_s(buf, L"Opacity: %d%%", pos);
            SetWindowText(m_hLblSubOpacity, buf);

            InvalidateRect(m_canvasWindow.GetHWND(), NULL, FALSE);
        }
        return 0;
    }

    case WM_COMMAND:
        OnCommand(wParam, lParam);
        UpdateTitleBar();
        return 0;

    case WM_USER + 100:
    {
        float x = static_cast<float>((int)wParam);
        float y = static_cast<float>((int)lParam);
        const std::wstring& msg = m_canvasWindow.GetEditorController().GetLastStatusMessage();
        UpdateStatusBar(x, y, msg.c_str());
        return 0;
    }

    case WM_USER + 101:
    {
        ToolType currentTool = m_toolManager.GetCurrentTool();
        UpdateToolUIState(currentTool);
        return 0;
    }

    case WM_SETFOCUS:
        if (m_canvasWindow.GetHWND())
        {
            SetFocus(m_canvasWindow.GetHWND());
        }
        return 0;

    case WM_CLOSE:
        if (PromptSaveIfDirty())
        {
            DestroyWindow(m_hWnd);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}