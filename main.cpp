#include "MainWindow.h"
#include "resource.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    MainWindow appWindow;

    if (!appWindow.Create(
        L"Direct2D C++ Object-Oriented App (D2Draw02)",
        WS_OVERLAPPEDWINDOW,
        0,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1024, 768
    ))
    {
        MessageBox(NULL, L"메인 윈도우 생성 실패!", L"오류", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(appWindow.GetHWND(), nCmdShow);
    UpdateWindow(appWindow.GetHWND());

    // 🚀 Professional Viewport / Active Rendering Loop (PeekMessage)
    MSG msg = {};
    bool bRunning = true;

    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    while (bRunning)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bRunning = false;
            }
            else
            {
                if (!hAccel || !TranslateAccelerator(appWindow.GetHWND(), hAccel, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
        else
        {
            QueryPerformanceCounter(&currentTime);
            float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
            lastTime = currentTime;

            // 너무 긴 프레임 딜레이 방지 (최대 0.1초)
            if (deltaTime > 0.1f) deltaTime = 0.1f;

            // 처리할 메시지가 없을 때 (Idle 타임)
            // 1) 상태 업데이트
            appWindow.GetCanvas().Update(deltaTime);
            // 2) 렌더링
            appWindow.GetCanvas().Render();

            // CPU 100% 방지 및 프레임 레이트 조절 (약 60 FPS 유지)
            Sleep(16);
        }
    }

    return (int)msg.wParam;
}
