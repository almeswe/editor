#include "gap.h";
#include "renderer.h"
#include <windows.h>

Gap* gap;
Renderer* renderer;

BOOL RepaintWindow(HWND window)
{
    return RedrawWindow(window, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
}

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            renderer = new Renderer(window);
            gap = new Gap();
            break;

        case WM_MOUSEWHEEL:
            if (GetKeyState(VK_CONTROL) < 0)
                renderer->OnScroll(HIWORD(wParam));
            break;

        case WM_KEYDOWN:
            //rewrite
            if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(0x56) < 0)
            {
                gap->InsertAt(gap->GetPoint(), L"pasted");
                break;
            }
            switch (wParam)
            {
                case VK_BACK:
                {
                    gap->RemoveAt(gap->GetPoint());
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
            gap->InsertAt(gap->GetPoint(), wParam);
            break;

        case WM_SIZE:
            renderer->OnResize(LOWORD(lParam),HIWORD(lParam));
            break;

        case WM_PAINT:
            PAINTSTRUCT ps;
            BeginPaint(window,&ps);
            renderer->SetText(gap->GetText());
            renderer->RenderText();
            renderer->RenderCursor(gap->GetPoint());
            EndPaint(window,&ps);
            return 0;

        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(window, message, wParam, lParam);
    }
    RepaintWindow(window);
}
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
