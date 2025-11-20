#pragma once

#include <windows.h>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

class RadarWindow {
private:
    HWND hwnd;
    std::vector<std::pair<int, int>> players;
    std::mutex playersMutex;
    std::atomic<bool> isRunning;
    std::thread windowThread;

    static bool s_classRegistered;

public:
    RadarWindow();
    ~RadarWindow();

    void Init();
    void Clear();
    void AddPlayer(int x, int y);
    bool IsRunning() const { return isRunning; }
    void Close();

private:
    void WindowThreadProc();
    bool CreateWindowInternal();
    void MessageLoop();
    void DrawPoints(HDC hdc);

    bool RegisterWindowClass();
    void UnregisterWindowClass();

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};