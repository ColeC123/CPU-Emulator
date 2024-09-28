#include "stdio.h"
#include "windows.h"

// This enables a windows subsystem and allows unicode values to be used
// compile with clang testwindow.c -o testwindow -municode -mwindows -std=c23
// compile with gcc testwindow.c -o testwindow -municode -mwindows

int cursorXPos = 0;
int cursorYPos = 0;

HDC hdc;

int char_list_count = 0;
int char_list_max_count = 0;
wchar_t char_list[1000];

HANDLE console;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    AllocConsole();
    console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwmode = 0;
    GetConsoleMode(console, &dwmode);
    dwmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(console,  dwmode);

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Learn To Program Windows",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    PAINTSTRUCT ps;
    hdc = BeginPaint(hwnd, &ps);
    HBRUSH hbrush = CreateSolidBrush(RGB(173, 216, 230));
    FillRect(hdc, &ps.rcPaint, hbrush);
    EndPaint(hwnd, &ps);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    FreeConsole();
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
    switch (umsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hwnd, &ps);
            HBRUSH hbrush = CreateSolidBrush(RGB(173, 216, 230));
            FillRect(hdc, &ps.rcPaint, hbrush);
            EndPaint(hwnd, &ps);
        }
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN: {
            if (wParam == VK_SPACE) {
                char_list[char_list_count] = L' ';
                char_list_count++;
                char_list_max_count++;
            } else if (wParam == VK_BACK) {
                char_list_count--;
                char_list[char_list_count] = L' ';
            } else {
                char_list[char_list_count] = (wchar_t)wParam;
                char_list_count++;
                char_list_max_count++;
            }
            hdc = GetDC(hwnd);
            SetTextColor(hdc, RGB(146, 99, 156));
            SetBkColor(hdc, RGB(173, 216, 230));
            TextOutW(hdc, 0, 0, char_list, char_list_max_count);
            WriteConsoleW(console, L"\x1b[0;0H", 6, NULL, NULL);
            WriteConsoleW(console, char_list, char_list_max_count, NULL, NULL);
        }
            return 0;
    }

    return DefWindowProcW(hwnd, umsg, wParam, lParam);
}