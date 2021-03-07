#include "gap.h";
#include "drawer.h"
#include <windows.h>

Gap* gap;
DWTextDrawer* drawer;

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            drawer = new DWTextDrawer(window);
            gap = new Gap();
            break;

        case WM_MOUSEWHEEL:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                drawer->OnScroll(HIWORD(wParam));
                drawer->DWDrawText(gap->GetText());
            }
            break;

        case WM_KEYDOWN:
            //rewrite
            switch (wParam)
            {
                case VK_BACK:
                {
                    gap->RemoveAt(gap->GetPoint());
                    drawer->DWDrawText(gap->GetText());
                    break;
                }
                case VK_LEFT:
                    gap->MovePointBackward();
                    break;
                case VK_RIGHT:
                    gap->MovePointForward();
                    break;
            }
            break;

        case WM_CHAR:
            if (wParam == VK_BACK)
                break;
            gap->InsertAt(gap->GetPoint(),wParam);
            drawer->DWDrawText(gap->GetText());
            break;

        case WM_SIZE:
            drawer->OnResize(LOWORD(lParam),HIWORD(lParam));
            drawer->DWDrawText(gap->GetText());
            break;

        case WM_PAINT:
            PAINTSTRUCT ps;
            BeginPaint(window,&ps);
            drawer->DWDrawText(gap->GetText());
            EndPaint(window,&ps);
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(window, message, wParam, lParam);
    }
    return 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    MSG message;
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.lpszClassName = L"ed";
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

    RegisterClassEx(&wc);
    HWND window = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"ed",
        L"editor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        instance,
        NULL
    );
    ShowWindow(window, SW_SHOW);
    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return message.wParam;
}