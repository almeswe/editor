#include <windows.h>

#include "drawer.h"
#include "gap.h";
#include "wchmethods.h"

PAINTSTRUCT ps;

Gap* gap;
DWTextDrawer* drawer;
wchar_t* text = const_cast<wchar_t*>(L"");

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            drawer = new DWTextDrawer(window);
            gap = new Gap();
            //gap->Insert(9,L"wut??");
            //gap->Insert(0,L"first");
            //text = gap->Text;
            break;

        case WM_KEYDOWN:
            //rewrite
            if (wParam == VK_BACK)
            {
                text = remlst(text);
                drawer->DWDrawText(text);
            }
            if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_ADD) < 0)
            {
                drawer->OnScroll(5.0f);
                drawer->DWDrawText(text);
            }
            if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SUBTRACT) < 0)
            {
                drawer->OnScroll(-5.0f);
                drawer->DWDrawText(text);
            }
            break;

        case WM_CHAR:
            text = append(text, wParam);
            drawer->DWDrawText(text);
            break;

        case WM_SIZE:
            drawer->OnResize(LOWORD(lParam),HIWORD(lParam));
            drawer->DWDrawText(text);
            break;

        case WM_PAINT:
            BeginPaint(window,&ps);
            drawer->DWDrawText(text);
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