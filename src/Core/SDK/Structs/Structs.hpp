#include <vector>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <psapi.h>
#include <regex>
#include <string>
#pragma comment(lib, "ntdll.lib")

extern "C" NTSYSCALLAPI NTSTATUS ZwReadVirtualMemory(
	HANDLE  hProcess,
	LPCVOID lpBaseAddress,
	LPVOID  lpBuffer,
	SIZE_T  nSize,
	SIZE_T * lpNumberOfBytesRead
);

extern "C" NTSYSCALLAPI NTSTATUS ZwWriteVirtualMemory(
	HANDLE  hProcess,
	LPVOID  lpBaseAddress,
	LPCVOID lpBuffer,
	SIZE_T  nSize,
	SIZE_T * lpNumberOfBytesWritten
);