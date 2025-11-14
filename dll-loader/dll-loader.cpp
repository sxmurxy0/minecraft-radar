#include <iostream>
#include <windows.h>
#include <string>
#include <io.h>

const char* DLL_FILE_NAME = "minecraft-radar.dll";
const char* WINDOW_CLASS_NAME = "GLFW30";
const char* WINDOW_TITLE = "Minecraft 1.21.4";

void injectDLL() {
	char dllFilePath[MAX_PATH];
	DWORD processID;

	GetFullPathNameA((LPCSTR)DLL_FILE_NAME, MAX_PATH, (LPSTR)dllFilePath, nullptr);

	if (_access(dllFilePath, 0) == -1) {
		std::cout << "Error: dll file not found!" << std::endl;
		return;
	}

	HWND wHandle = FindWindowA((LPSTR)WINDOW_CLASS_NAME, (LPSTR)WINDOW_TITLE);
	if (!wHandle) {
		std::cout << "Error: window not found!" << std::endl;
		return;
	}

	GetWindowThreadProcessId(wHandle, &processID);
	
	if (!processID) {
		std::cout << "Error: failed to get process id!" << std::endl;
		return;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processID);
	if (!hProcess) {
		std::cout << "Error: failed to open handle to process!" << std::endl;
		return;
	}

	LPVOID ptr = VirtualAllocEx(hProcess, nullptr, MAX_PATH,
		MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (ptr) {
		if (WriteProcessMemory(hProcess, ptr, dllFilePath, MAX_PATH, nullptr)) {
			HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
				LPTHREAD_START_ROUTINE(LoadLibraryA), ptr, 0, nullptr);
			if (hThread) {
				WaitForSingleObject(hThread, INFINITE);
				CloseHandle(hThread);

				std::cout << "Ok. No errors occurred.\nDLL successfully loaded!" << std::endl;
			}
			else {
				std::cout << "Error: failed to create remote thread!" << std::endl;
			}
		}
		else {
			std::cout << "Error: failed to write to process memory!" << std::endl;
		}

		VirtualFreeEx(hProcess, ptr, 0, MEM_RELEASE);
	}
	else {
		std::cout << "Error: failed to allocate process memory!" << std::endl;
	}

	CloseHandle(hProcess);
}

int main() {
	injectDLL();
	
	std::cout << "\nPress Enter to close console..." << std::endl;
	std::cin.get();
	
	return 0;
}