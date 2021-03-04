#include <windows.h>

#include "DWTextWriter.cpp"

DWTextDrawer* drawer;
wchar_t* text = const_cast<wchar_t*>(L"privet mir");

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            //text = (char*)wParam;
            break;

        case WM_PAINT:
        case WM_DISPLAYCHANGE:
            drawer->DWDrawText(text);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
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
    HWND window = CreateWindowEx(WS_EX_CLIENTEDGE, L"ed", L"editor", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, instance, NULL);
    drawer = new DWTextDrawer(window);

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return message.wParam;
}