#include <windows.h>
#include <iostream>
#include <chrono>
#include <ctime>


HWND hwnd;
int window_length = 70, window_height = 60;
BOOL trackerIsOn = FALSE;
RECT rect = {0, 0, window_length, window_height};
RECT previousWindowPosition;

BOOL mousedown = FALSE;
POINT lastLocation;

std::chrono::system_clock::time_point startTimePoint = std::chrono::system_clock::now();

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        if (0 == wParam) // becoming inactive, put on top
        {
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        break;
    case WM_TIMER:
    {
        // Handle the timer event (update time)
        InvalidateRect(hwnd, NULL, TRUE); // Force repaint
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        break;
    }
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

        // Get the current time
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

        // Convert to a string (hours and minutes only)
        char timeStr[64];
        struct tm timeInfo;
        localtime_s(&timeInfo, &currentTime);
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeInfo);

        // Display the time
        TextOutA(hdc, 7, 7, timeStr, static_cast<int>(strlen(timeStr)));

        // Calculate the duration
        auto duration = now - startTimePoint;

        char durStr[64];
        // Convert duration to hours, minutes, and seconds
        auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
        duration -= hours;
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
        duration -= minutes;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

        // Format the duration as HH:MM:SS
        // char durStr[9]; // Space for "HH:MM:SS\0"
        snprintf(durStr, sizeof(durStr), "%02d:%02d:%02d", hours.count(), minutes.count(), seconds.count());
        // Display the duration
        TextOutA(hdc, 7, 35, durStr, static_cast<int>(strlen(durStr)));
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        mousedown = true;
        GetWindowRect(hwnd, &previousWindowPosition);
        GetCursorPos(&lastLocation);
        RECT rect;
        GetWindowRect(hwnd, &rect);
        lastLocation.x = lastLocation.x - rect.left;
        lastLocation.y = lastLocation.y - rect.top;
        break;
    }
    case WM_LBUTTONUP:
    {
        mousedown = false;
        RECT currentWindowPosition;
        GetWindowRect(hwnd, &currentWindowPosition);
        if (abs(currentWindowPosition.left - previousWindowPosition.left) < 3 && abs(currentWindowPosition.top - previousWindowPosition.top) < 3)
        {
            trackerIsOn = !trackerIsOn;
            InvalidateRect(hwnd, &rect, TRUE);

            if (trackerIsOn)
            {
                startTimePoint = std::chrono::system_clock::now();
            }
        }

        break;
    }
    case WM_MOUSEMOVE:
    {
        if (mousedown)
        {
            POINT currentpos;
            GetCursorPos(&currentpos);
            int x = currentpos.x - lastLocation.x;
            int y = currentpos.y - lastLocation.y;
            MoveWindow(hwnd, x, y, window_length, window_height, false);
        }
        break;
    }
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

    hwnd = CreateWindowW(L"MyWindowClass", L"timeTracker", WS_POPUP,
                         CW_USEDEFAULT, CW_USEDEFAULT, window_length, window_length, NULL, NULL, hInstance, NULL);

    // Set up a timer to fire every second.  1 minute (60,000 milliseconds)
    SetTimer(hwnd, 1, 1000, NULL);
    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
