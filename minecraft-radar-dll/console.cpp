#include "console.h"
#include <iostream>

Console::Console() : hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {}

Console::~Console() {
    fclose(stdout);
    FreeConsole();
}

void Console::Init() {
    AllocConsole();
    
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);

    std::cout.clear();
    
    hwnd = GetConsoleWindow();
}

void Console::Show(bool show) {
    ShowWindow(hwnd, show ? SW_SHOW : SW_HIDE);
}

void Console::Print(const char* format, ...) {
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    std::cout << std::endl;
}