#pragma once
#include <Gui/Overlay/Overlay.hpp>
#include <Gui/gui.hpp>
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <core/core.hpp>
#include <core/sdk/Memory.hpp>
#include <core/sdk/sdk.hpp>


#include <dwmapi.h>
#include <regex>
#include <tchar.h>
#include <vector>
#include <windows.h>
#include <winternl.h>


#include <Includes/Logger.hpp>
#include <Security/AntiCrack.hpp>
#include <csignal>

using namespace core;

std::string WStringToString(const std::wstring &wstr) {
  if (wstr.empty())
    return {};

  int size_needed = WideCharToMultiByte(
      CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
  std::string result(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), result.data(),
                      size_needed, nullptr, nullptr);
  return result;
}

LONG WINAPI CrashHandler(EXCEPTION_POINTERS *pExceptionInfo) {
  DWORD code = pExceptionInfo->ExceptionRecord->ExceptionCode;
  PVOID addr = pExceptionInfo->ExceptionRecord->ExceptionAddress;
  char buf[256];
  sprintf_s(buf, "FATAL CRASH: Exception 0x%08X at address 0x%p", code, addr);
  Logger::Log("CRASH", buf);
  return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[]) {

  Logger::Init();
  SetUnhandledExceptionFilter(CrashHandler);
  Logger::Log("MAIN", "Application starting...");

  HANDLE hMutex = CreateMutexA(nullptr, TRUE, xorstr("secure_mutex"));
  if (!hMutex) {
    Logger::Log("MAIN", "ERROR: Failed to create mutex");
    MessageBoxA(nullptr, xorstr("Failed to create mutex."), xorstr("Error"),
                MB_OK | MB_ICONERROR);
    return 0;
  }

  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    Logger::Log("MAIN", "ERROR: Cheat is already running (mutex exists)");
    MessageBoxA(nullptr, xorstr("Cheat is already running!"), xorstr("Error"),
                MB_OK | MB_ICONERROR);
    CloseHandle(hMutex);
    return 0;
  }

  if (!driver.GetMaxPrivileges(GetCurrentProcess())) {
    Logger::Log("MAIN", "ERROR: Failed to get admin privileges");
    MessageBoxA(
        nullptr,
        xorstr(
            "Failed to get privileges. Please run the cheat as Administrator."),
        xorstr("Error"), MB_OK | MB_ICONERROR);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
  }

  Logger::Log("MAIN", "Privileges OK. Searching for FiveM window...");

  constexpr int maxWaitMs = 30000;
  int waitedMs = 0;
  while (!g_Variables.g_hGameWindow && waitedMs < maxWaitMs) {
    g_Variables.g_hGameWindow = FindWindowA(xorstr("grcWindow"), nullptr);
    if (g_Variables.g_hGameWindow) {
      auto WindowInfo = Utils::GetWindowPosAndSize(g_Variables.g_hGameWindow);
      g_Variables.g_vGameWindowPos = WindowInfo.first;
      g_Variables.g_vGameWindowSize = WindowInfo.second;
      g_Variables.g_vGameWindowCenter = {g_Variables.g_vGameWindowSize.x / 2,
                                         g_Variables.g_vGameWindowSize.y / 2};
      Logger::Log("MAIN", "Found FiveM window!");
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    waitedMs += 500;
  }

  if (!g_Variables.g_hGameWindow) {
    Logger::Log("MAIN", "ERROR: FiveM window not found after 30s");
    MessageBoxA(nullptr,
                xorstr("Could not find FiveM window (grcWindow). Please start "
                       "the game first!"),
                xorstr("Error"), MB_OK | MB_ICONERROR);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
  }

  DWORD ProcIdFiveM = 0;
  GetWindowThreadProcessId(g_Variables.g_hGameWindow, &ProcIdFiveM);
  if (!ProcIdFiveM) {
    MessageBoxA(nullptr, xorstr("Could not get FiveM process ID."),
                xorstr("Error"), MB_OK | MB_ICONERROR);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
  }

  std::string processName = driver.GetNameByPid(ProcIdFiveM);
  if (processName.empty()) {
    MessageBoxA(nullptr, xorstr("Could not find process name for FiveM PID."),
                xorstr("Error"), MB_OK | MB_ICONERROR);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
  }

  Logger::Log("MAIN", "Opening process: " + processName);
  if (!driver.OpenProc(processName.c_str())) {
    Logger::Log("MAIN", "ERROR: Failed to open FiveM process");
    MessageBoxA(nullptr,
                xorstr("Failed to open FiveM process. Are you running the "
                       "cheat as Administrator?"),
                xorstr("Error"), MB_OK | MB_ICONERROR);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
  }

  if (!driver.ModBase) {
    Logger::Log("MAIN", "ERROR: ModBase is null");
    MessageBoxA(
        nullptr,
        xorstr("Looks like there was an error Please restart your computer."),
        xorstr("Warning"), MB_OK | MB_ICONWARNING);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
  }
  Logger::Log("MAIN", "ModBase: 0x" + std::to_string(driver.ModBase));

  uintptr_t modSize = 0;
  driver.name_module_base = driver.GetModuleBaseAddr(
      ProcIdFiveM, xorstr("citizen-playernames-five.dll"), &modSize);
  driver.glue_dll =
      driver.GetModuleBaseAddr(ProcIdFiveM, xorstr("glue.dll"), &modSize);

  offsets.CurrentBuild = 3258;

  GetOffsets();
  Logger::Log("MAIN", "Offsets resolved. Starting overlay render...");

  Gui::cOverlay.Render();

  Logger::Log("MAIN", "Overlay::Render() returned. Application shutting down.");
  ReleaseMutex(hMutex);
  CloseHandle(hMutex);

  return 0;
}