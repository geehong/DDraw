#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "BaseWindow.h"
#include "CanvasWindow.h"
#include "Editing/ToolManager.h"
#include "UI/DockPanel.h"
#include "UI/DockingManager.h"
#include <commctrl.h>
#include <string>

class MainWindow : public BaseWindow
{
private:
    ToolManager     m_toolManager;
    CanvasWindow    m_canvasWindow;
    DockingManager  m_dockingManager;
    DockPanel       m_leftDockPanel;
    DockPanel       m_rightDockPanel;

    HWND            m_hToolBar;
    HWND            m_hStatusBar;
    HWND          m_hLeftPanel;     // Softimage Left Tool Panel
    HWND          m_hRightMCP;      // Softimage Right MCP (Main Command Panel)

    // Left Panel Win32 Controls
    HWND          m_hLblToolsHeader;
    HWND          m_hBtnSelect;
    HWND          m_hBtnRect;
    HWND          m_hBtnEllipse;
    HWND          m_hBtnLine;
    HWND          m_hBtnText;
    HWND          m_hBtnPolygon;
    HWND          m_hLblActionsHeader;
    HWND          m_hBtnUndo;
    HWND          m_hBtnRedo;
    HWND          m_hBtnDelete;

    // Right Panel Win32 Property Controls
    HWND          m_hGrpShapeProperties;
    HWND          m_hLblLineWidth;
    HWND          m_hTrackStrokeWidth;
    HWND          m_hBtnStrokeColor;
    HWND          m_hBtnFillColor;
    HWND          m_hChkFilled;

    // Grid Settings UI Controls
    HWND          m_hGrpGridSettings;
    HWND          m_hLblGridUnit;
    HWND          m_hComboGridUnit;
    HWND          m_hChkShowGrid;

    // [Main Grid GroupBox & Controls]
    HWND          m_hGrpMainGrid;
    HWND          m_hLblMainGridStep;
    HWND          m_hComboMainGridStep;
    HWND          m_hLblMainLineWidth;
    HWND          m_hTrackMainLineWidth;
    HWND          m_hBtnMainGridColor;
    HWND          m_hLblMainOpacity;
    HWND          m_hTrackMainOpacity;

    // [Sub Grid GroupBox & Controls]
    HWND          m_hGrpSubGrid;
    HWND          m_hLblSubDivisions;
    HWND          m_hComboSubDivisions;
    HWND          m_hLblSubLineWidth;
    HWND          m_hTrackSubLineWidth;
    HWND          m_hBtnSubGridColor;
    HWND          m_hLblSubOpacity;
    HWND          m_hTrackSubOpacity;

    HWND          m_hChkSnapToGrid;

    HWND          m_hLblSelectionInfo;

    std::wstring  m_currentFileName;
    std::wstring  m_currentFilePath;
    bool          m_bIsDirty;

public:
    MainWindow();
    virtual ~MainWindow();

    CanvasWindow& GetCanvas() { return m_canvasWindow; }
    ToolManager& GetToolManager() { return m_toolManager; }

    void UpdateTitleBar();
    void UpdateStatusBar(float x, float y, const wchar_t* statusText = nullptr);
    void UpdateSelectionPropertiesUI();

protected:
    virtual PCWSTR GetClassName() const override { return L"D2DMainWindow"; }
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
    void OnCreate();
    void OnSize(UINT width, UINT height);
    void OnCommand(WPARAM wParam, LPARAM lParam);
    void CreateAppMenu();
    void CreateAppToolBar();
    void CreateAppStatusBar();
    void CreateSoftimagePanels();
    void LayoutPanelControls();
    void UpdateToolUIState(ToolType tool);
    void ApplyPropertyChanges();

    bool PromptSaveIfDirty();
    void FileNew();
    void FileOpen();
    bool FileSave();
    bool FileSaveAs();
};

#endif // MAIN_WINDOW_H

