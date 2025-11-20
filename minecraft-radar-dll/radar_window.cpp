#include "radar_window.h"

bool RadarWindow::s_classRegistered = false;

RadarWindow::RadarWindow() : hwnd(nullptr), isRunning(false) {}

RadarWindow::~RadarWindow() {
    Close();
}

bool RadarWindow::RegisterWindowClass() {
    if (s_classRegistered) return true;

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"RadarWindowClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    s_classRegistered = (RegisterClassEx(&wc) != 0);
    return s_classRegistered;
}

void RadarWindow::UnregisterWindowClass() {
    UnregisterClass(L"RadarWindowClass", GetModuleHandle(NULL));
}

void RadarWindow::Init() {
    if (!RegisterWindowClass()) {
        return;
    }

    isRunning = true;
    windowThread = std::thread(&RadarWindow::WindowThreadProc, this);
}

void RadarWindow::WindowThreadProc() {
    if (CreateWindowInternal()) {
        MessageLoop();
    }
}

bool RadarWindow::CreateWindowInternal() {
    hwnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        L"RadarWindowClass",
        L"Radar++",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        100, 100, 400, 400,
        NULL, NULL, GetModuleHandle(NULL), this
    );

    if (!hwnd) {
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return true;
}

void RadarWindow::MessageLoop() {
    MSG msg = {0};

    while (isRunning) {
        BOOL result = GetMessage(&msg, NULL, 0, 0);
        if (result <= 0) {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    isRunning = false;
}

void RadarWindow::AddPlayer(int x, int y) {
    std::lock_guard<std::mutex> lock(playersMutex);
    players.push_back({x, y});

    if (hwnd) {
        PostMessage(hwnd, WM_USER + 1, 0, 0);
    }
}

void RadarWindow::Clear() {
    std::lock_guard<std::mutex> lock(playersMutex);
    players.clear();

    if (hwnd) {
        PostMessage(hwnd, WM_USER + 1, 0, 0);
    }
}

void RadarWindow::Close() {
    isRunning = false;

    if (hwnd) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }

    UnregisterWindowClass();

    if (windowThread.joinable()) {
        windowThread.join();
    }
}

void RadarWindow::DrawPoints(HDC hdc) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    FillRect(hdc, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    
    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

    int centerX = clientRect.right / 2;
    int centerY = clientRect.bottom / 2;

    MoveToEx(hdc, centerX, 0, NULL);
    LineTo(hdc, centerX, clientRect.bottom);
    MoveToEx(hdc, 0, centerY, NULL);
    LineTo(hdc, clientRect.right, centerY);

    int radarRadius = min(centerX, centerY) - 10;
    Arc(hdc, centerX - radarRadius, centerY - radarRadius, 
            centerX + radarRadius, centerY + radarRadius, 0, 0, 0, 0);

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);

    std::vector<std::pair<int, int>> playersCopy;
    {
        std::lock_guard<std::mutex> lock(playersMutex);
        playersCopy = players;
    }

    if (!playersCopy.empty()) {
        HPEN whitePen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
        HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));

        HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));

        oldPen = (HPEN)SelectObject(hdc, redPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, redBrush);

        int radius = 5;

        for (const auto& point : playersCopy) {
            int displayX = centerX + point.first;
            int displayY = centerY + point.second;

            int dx = displayX - centerX;
            int dy = displayY - centerY;
            int distance = static_cast<int>(sqrt(dx * dx + dy * dy));

            if (distance > radarRadius - radius) {
                double angle = atan2(dy, dx);
                displayX = centerX + static_cast<int>((radarRadius - 5) * cos(angle));
                displayY = centerY + static_cast<int>((radarRadius - 5) * sin(angle));
            }

            Ellipse(hdc,
                displayX - radius, displayY - radius,
                displayX + radius, displayY + radius);
        }

        SelectObject(hdc, whitePen);
        SelectObject(hdc, whiteBrush);

        Ellipse(hdc,
            centerX - 5, centerY - 5,
            centerX + 5, centerY + 5);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(redPen);
        DeleteObject(redBrush);
    }
}

LRESULT CALLBACK RadarWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    RadarWindow* window = nullptr;

    if (msg == WM_CREATE) {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        window = (RadarWindow*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
    }
    else {
        window = (RadarWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (!window) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            window->DrawPoints(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_USER + 1: {
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_ERASEBKGND: {
            return 1;
        }
        default: {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }
}