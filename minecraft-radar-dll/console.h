#pragma once

#include <windows.h>
#include <cstdio>
#include <cstdarg>

class Console {
private:
    HANDLE hConsole = nullptr;
    HWND hwnd = nullptr;

public:
    Console();
    ~Console();
    
    void Init();
    void Show(bool show);
    void Print(const char* format, ...);
};

extern Console g_Console;