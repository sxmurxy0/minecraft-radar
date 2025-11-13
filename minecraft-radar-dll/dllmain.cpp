#include <windows.h>
#include <jni.h>
#include "console.h"
#include "entity_provider.h"
#include "list_wrapper.h"

JavaVM* g_JavaVM = nullptr;
JNIEnv* g_JniEnv = nullptr;

Console g_Console;
PlayerProvider g_PlayerProvider;

bool SetupJNIEnv() {
    jsize vmCount;
    if (JNI_GetCreatedJavaVMs(&g_JavaVM, 1, &vmCount) != JNI_OK || vmCount == 0) {
        return FALSE;
    }

    if (g_JavaVM->AttachCurrentThread((void**)&g_JniEnv, nullptr) != JNI_OK) {
        return FALSE;
    }

    return TRUE;
}

DWORD WINAPI DllThreadProc(HMODULE hModule) {
    if (!SetupJNIEnv()) {
        MessageBox(0, L"Failed to attach DLL!", L"Error", MB_ICONERROR);
        FreeLibraryAndExitThread(hModule, 0);

        return 0;
    }

    MessageBox(0, L"DLL successfully attached!", L"Info", MB_ICONINFORMATION);

    g_Console.Init();
    g_PlayerProvider.Init();

    while (!(GetAsyncKeyState(VK_DELETE) & 1)) {
        jobject player = g_PlayerProvider.GetLocalPlayerObject();
        if (player) {
            auto localPos = g_PlayerProvider.GetEntityPosition(player);
            std::string posString = "Self: " + std::to_string(localPos.first) + 
                   + " " + std::to_string(localPos.second);
            g_Console.Print(posString.c_str());
            
            jobject playersList = g_PlayerProvider.GetPlayerListObject();
            ListWrapper list(playersList);
            for (int i = 0; i < list.GetSize(); i++) {
                jobject entity = list.GetElement(i);
                auto pos = g_PlayerProvider.GetEntityPosition(entity);
                double dx = pos.first - localPos.first, dy = pos.second - localPos.second;
                double distance = sqrt(dx * dx + dy * dy);
                posString = "Entity" + std::to_string(i) + ": " + std::to_string(pos.first) +
                    + " " + std::to_string(pos.second) + " | Distance: " + std::to_string(distance);
                g_Console.Print(posString.c_str());
            }
        }

        Sleep(1000);
    }

    g_JavaVM->DetachCurrentThread();

    MessageBox(0, L"DLL unloaded!", L"Info", MB_ICONINFORMATION);
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        HANDLE tHandle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DllThreadProc, hModule, 0, nullptr);
        if (tHandle) {
            CloseHandle(tHandle);
        }
    }

    return TRUE;
}
