#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winuser.h>

long totalX = 0;
long totalY = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    if (msg == WM_INPUT) {
        UINT size = 0;

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

        BYTE* buffer = (BYTE*)malloc(size);

        if (buffer != NULL) {
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) != (UINT)-1) {
                RAWINPUT* raw = (RAWINPUT*)buffer;

                if (raw->header.dwType == RIM_TYPEMOUSE) {
                    long dx = raw->data.mouse.lLastX;
                    long dy = raw->data.mouse.lLastY;

                    totalX += dx;
                    totalY += dy;
                }

                char titleBuffer[256];
                sprintf_s(titleBuffer, sizeof(titleBuffer), "X %ld | Y %ld", totalX, totalY);

                SetWindowTextA(hwnd, titleBuffer);
            }

            free(buffer);
        }
    }

    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG msg;
    BOOL messageExists;


    WNDCLASSA wc = {0};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.lpszClassName = "MouseTracker";

    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Error registering window class", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowExA(
        0,
        "MouseTracker",
        "Mouse Movement Tracker",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 200,
        NULL,
        NULL,
        instance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBoxA(NULL, "Error creating window", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;

    if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
        MessageBoxA(NULL, "Error adding raw input device", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    while ((messageExists = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (messageExists == -1) {
            MessageBoxA(NULL, "Error fetching message", "Error", MB_OK | MB_ICONERROR);
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
