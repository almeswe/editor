#include "gap.h";
#include "renderer.h"
#include "clipboard.h"

#include <windows.h>

Gap* gap;
Renderer* renderer;

Clipboard cp;

BOOL RepaintWindow(HWND window)
{
    return RedrawWindow(window, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
}

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            cp.SetMainHandle(window);
            renderer = new Renderer(window);
            gap = new Gap();
            break;

        case WM_MOUSEWHEEL:
            if (GetKeyState(VK_CONTROL) < 0)
                renderer->OnCtrlScroll(HIWORD(wParam));
            else
                renderer->OnScroll(HIWORD(wParam));
            break;

        case WM_LBUTTONDOWN:
            renderer->OnMouseClick(LOWORD(lParam),HIWORD(lParam),gap);
            break;

        case WM_KEYDOWN:
            if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(0x56) < 0)
            {
                if (SUCCEEDED(cp.ReadData()))
                    gap->InsertAt(gap->GetCursorPosition(),cp.Text);
                break;
            }
            switch (wParam)
            {
                case VK_BACK:
                    gap->RemoveAt(gap->GetCursorPosition());
                    break;

                case VK_UP:
                    gap->MoveCursorUp();
                    break;

                case VK_DOWN:
                    gap->MoveCursorDown();
                    break;

                case VK_LEFT:
                    gap->MoveCursorBackward();
                    break;

                case VK_RIGHT:
                    gap->MoveCursorForward();
                    break;

                case VK_RETURN:
                    gap->InsertAt(gap->GetCursorPosition(), L"\n");
                    break;
            }
            break;

        case WM_CHAR:
            switch (wParam)
            {
                case VK_BACK:
                case VK_RETURN:
                case '\x16':
                    break;
                default:
                    gap->InsertAt(gap->GetCursorPosition(), wParam);
            }
            break;

        case WM_SIZE:
            renderer->OnResize(LOWORD(lParam),HIWORD(lParam));
            break;

        case WM_PAINT:
            PAINTSTRUCT ps;
            BeginPaint(window,&ps);
            renderer->RenderText(gap);
            renderer->RenderCursor(gap);
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
