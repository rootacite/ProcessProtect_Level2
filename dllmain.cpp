// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "def.h"

#include "detours/detours.h"
#pragma comment(lib,"detours.lib")

#pragma data_seg("SHARED")

HHOOK  hhk = NULL;
DLLEXPORT DWORD ProtectedPID = 0;

#pragma data_seg()
#pragma comment(linker, "/section:SHARED,rws")




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        LocalBanProcessOperation();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        LocalUnBanProcessOperation();
        break;
    }
    return TRUE;
}

void StartHook(HMODULE hModule)
{
    hhk = SetWindowsHookEx(WH_CALLWNDPROC, WndProc, hModule, 0);
}

void EndHook()
{
    UnhookWindowsHookEx(hhk);
}

LRESULT CALLBACK WndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(hhk, nCode, wParam, lParam);
}
HANDLE WINAPI fakeOpenProcess(DWORD a, BOOL isi, DWORD pid)
{
    if (pid != ProtectedPID)return pOpenProcess(a, isi, pid);

    a = a & ~(PROCESS_TERMINATE | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME);
    return pOpenProcess(a, isi, pid);
}
void LocalBanProcessOperation()
{
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pOpenProcess, fakeOpenProcess);
    DetourTransactionCommit();
}

void LocalUnBanProcessOperation()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)pOpenProcess, fakeOpenProcess);
    DetourTransactionCommit();
}

