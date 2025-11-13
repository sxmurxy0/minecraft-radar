#pragma once

#include <windows.h>
#include <cstdio>
#include <cstdarg>

class Console {
private:
    HANDLE hConsole = nullptr;

public:
    Console() : hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {}
    ~Console();
    
    void Init();

    void Print(const char* format, ...);
};

extern Console g_Console;