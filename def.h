#pragma once

#include "pch.h"

DLLEXPORT void StartHook(HMODULE hModule);
DLLEXPORT void EndHook();
DLLEXPORT LRESULT CALLBACK WndProc(int nCode, WPARAM wParam, LPARAM lParam);

typedef HANDLE(WINAPI* POpenProcessType)(DWORD, BOOL, DWORD);

POpenProcessType pOpenProcess = OpenProcess;

HANDLE WINAPI fakeOpenProcess(DWORD a, BOOL isi, DWORD pid);


void LocalBanProcessOperation();
void LocalUnBanProcessOperation();