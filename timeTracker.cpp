#include <windows.h>

BOOL trackerIsOn = FALSE;
RECT rect = {5, 5, 55, 35};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        const COLORREF rgbGreen = 0x0000FF00;
        const COLORREF rgbBackground = GetSysColor(COLOR_WINDOW);
        HBRUSH hbr;

        if (trackerIsOn)
        {
            hbr = CreateSolidBrush(rgbGreen);
        }
        else
        {
            hbr = CreateSolidBrush(rgbBackground);
        }
        FillRect(hdc, &rect, hbr);
        DeleteObject(hbr);

        TextOutW(hdc, 10, 10, L"123!", 3); // Use TextOutW for wide-character strings
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_LBUTTONDOWN:
        trackerIsOn = !trackerIsOn;
        InvalidateRect(hwnd, &rect, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam); // Use DefWindowProcW
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSW wc = {0}; // Use WNDCLASSW for wide-character strings
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindowClass"; // Prefix with L for wide-character string
    RegisterClassW(&wc);                 // Use RegisterClassW

    HWND hwnd = CreateWindowW(L"MyWindowClass", L"Hello, Windows desktop123!", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
