#include <windows.h>
#include <jni.h>
#include "console.h"
#include "player_provider.h"
#include "list_wrapper.h"
#include "radar_window.h"

JavaVM* g_JavaVM = nullptr;
JNIEnv* g_JniEnv = nullptr;

Console g_Console;
PlayerProvider g_PlayerProvider;
RadarWindow g_RadarWindow;

bool SetupJNIEnv() {
    jsize vmCount;
    if (JNI_GetCreatedJavaVMs(&g_JavaVM, 1, &vmCount) != JNI_OK || vmCount == 0) {
        return false;
    }

    if (g_JavaVM->AttachCurrentThread((void**)&g_JniEnv, nullptr) != JNI_OK) {
        return false;
    }

    return true;
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
    g_RadarWindow.Init();

    g_Console.Show(false);

    while (!(GetAsyncKeyState(VK_DELETE) & 1)) {
        g_RadarWindow.Clear();

        jobject localPlayerObject = g_PlayerProvider.GetLocalPlayerObject();
        jobject playerListObject = g_PlayerProvider.GetPlayerListObject();

        if (localPlayerObject && playerListObject) {
            auto localPos = g_PlayerProvider.GetEntityPosition(localPlayerObject);

            ListWrapper playerList(playerListObject);
            for (int i = 0; i < playerList.Size(); i++) {
                jobject playerObject = playerList.Get(i);
                if (playerObject && playerObject != localPlayerObject) {
                    auto pos = g_PlayerProvider.GetEntityPosition(playerObject);

                    double dx = pos.first - localPos.first;
                    double dy = pos.second - localPos.second;

                    int radarX = static_cast<int>(dx * 2);
                    int radarY = static_cast<int>(dy * 2);

                    g_RadarWindow.AddPlayer(radarX, radarY);
                }
            }
        }

        Sleep(60);
    }

    g_RadarWindow.Close();
    g_JavaVM->DetachCurrentThread();

    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        HANDLE tHandle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DllThreadProc, hModule, 0, nullptr);
        if (tHandle) {
            CloseHandle(tHandle);
        }
    }

    return TRUE;
}