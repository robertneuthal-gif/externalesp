#pragma once
#include "Overlay.hpp"

#include <Gui/Gui.hpp>
#include <Gui/fonts/anta.h>
#include <Includes/Logger.hpp>
#include <core/core.hpp>
#include <dwmapi.h>
#include <tchar.h>
#include <thread>

#define FULLSCREEN_EX

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

namespace Gui {
typedef HWND(WINAPI *CreateWindowInBand)(
    _In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName,
    _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth,
    _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);

inline HHOOK hKeyboardHook;

inline LRESULT CALLBACK KeyboardCallBack(int nCode, WPARAM wParam,
                                         LPARAM lParam) {
  if (nCode >= 0) {
    KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT *)lParam;
    PostMessage(g_Variables.g_hCheatWindow, wParam, pKeyboard->vkCode, 0);
  }
  return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

static void supress_wow() { SystemParametersInfo(SPI_SETBEEP, FALSE, 0, 0); }

void Overlay::Render() {
#ifdef FULLSCREEN_EX
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  CreateWindowInBand pCreateWindowInBand =
      reinterpret_cast<CreateWindowInBand>(GetProcAddress(
          LoadLibraryA(xorstr("user32.dll")), xorstr("CreateWindowInBand")));
  PrepareForUIAccess();
#endif // FULLSCREEN_EX

  WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, Overlay::WndProc,
                    0L,         0L,         GetModuleHandle(NULL),
                    NULL,       NULL,       NULL,
                    NULL,       L" ",       NULL};
  ATOM RegClass = RegisterClassExW(&wc);

#ifdef FULLSCREEN_EX
  g_Variables.g_hCheatWindow = pCreateWindowInBand(
      WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, RegClass, L" ",
      WS_POPUP, static_cast<int>(g_Variables.g_vGameWindowPos.x),
      static_cast<int>(g_Variables.g_vGameWindowPos.y),
      static_cast<int>(g_Variables.g_vGameWindowSize.x),
      static_cast<int>(g_Variables.g_vGameWindowSize.y), NULL, NULL,
      wc.hInstance, NULL, 2 /*ZBID_UIACCESS*/);
#else
  g_Variables.g_hCheatWindow = CreateWindowExW(
      WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
      wc.lpszClassName, L" ", WS_POPUP, g_Variables.g_vGameWindowPos.x,
      g_Variables.g_vGameWindowPos.y, g_Variables.g_vGameWindowSize.x,
      g_Variables.g_vGameWindowSize.y, NULL, NULL, wc.hInstance, NULL);
#endif // FULLSCREEN_EX

  SetLayeredWindowAttributes(g_Variables.g_hCheatWindow, RGB(0, 0, 0), 255,
                             LWA_ALPHA);
  MARGINS Margin = {
      g_Variables.g_vGameWindowPos.x, g_Variables.g_vGameWindowPos.y,
      g_Variables.g_vGameWindowSize.x, g_Variables.g_vGameWindowSize.y};
  DwmExtendFrameIntoClientArea(g_Variables.g_hCheatWindow, &Margin);

  if (!CreateDeviceD3D(g_Variables.g_hCheatWindow)) {
    Logger::Log("OVERLAY", "ERROR: Failed to create D3D11 device");
    CleanupDeviceD3D();
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return;
  }
  Logger::Log("OVERLAY", "D3D11 device created successfully");
  SetWindowDisplayAffinity(g_Variables.g_hCheatWindow, g_Config.General->StreamProof ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);

  ShowWindow(g_Variables.g_hCheatWindow, SW_SHOWDEFAULT);
  UpdateWindow(g_Variables.g_hCheatWindow);

  ImGui::CreateContext();

  Style();
  Fonts();

  ImGui_ImplWin32_Init(g_Variables.g_hCheatWindow);
  ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

#ifdef FULLSCREEN_EX
  hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallBack, NULL, 0);
#endif
  supress_wow();

  core::StartThreads();
  if (!core::ThreadsStarted) {
    Logger::Log("OVERLAY",
                "ERROR: core::StartThreads() failed - ThreadsStarted is false");
    return;
  }
  Logger::Log("OVERLAY", "Threads started. Entering render loop...");

  static RECT old_rc;
  ZeroMemory(&Message, sizeof(MSG));

  bool overlayActive = true;
  HWND lastActiveWindow = nullptr;
  bool wasMenuOpen = false;

  static int frameCount = 0;
  static bool lastStreamProof = g_Config.General->StreamProof;

  while (Message.message != WM_QUIT) {
    try {

      if (lastStreamProof != g_Config.General->StreamProof) {
        SetWindowDisplayAffinity(g_Variables.g_hCheatWindow,
                                 g_Config.General->StreamProof
                                     ? WDA_EXCLUDEFROMCAPTURE
                                     : WDA_NONE);
        lastStreamProof = g_Config.General->StreamProof;
      }

      if (PeekMessageW(&Message, g_Variables.g_hCheatWindow, 0, 0, PM_REMOVE)) {

        if (Message.message == WM_SYSCHAR || Message.message == WM_SYSKEYDOWN ||
            Message.message == WM_SYSKEYUP) {
          continue;
        }
        TranslateMessage(&Message);
        DispatchMessage(&Message);
      }

      HWND ActiveWindow = GetForegroundWindow();

      // Window focus check
      bool isGameOrCheatActive = (ActiveWindow == g_Variables.g_hGameWindow ||
                                  ActiveWindow == g_Variables.g_hCheatWindow);

      if (!isGameOrCheatActive) {
        if (overlayActive) {
          overlayActive = false;
          ShowWindow(g_Variables.g_hCheatWindow, SW_HIDE);
          g_MenuInfo.IsOpen = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        continue;
      } else {
        if (!overlayActive) {
          overlayActive = true;
          ShowWindow(g_Variables.g_hCheatWindow, SW_SHOWDEFAULT);
          UpdateWindow(g_Variables.g_hCheatWindow);
        }
      }

      if (GetAsyncKeyState(g_Config.General->MenuKey) & 1) {

        if (g_MenuInfo.IsOpen == false) {
          if (isGameOrCheatActive) {
            g_MenuInfo.IsOpen = true;

#ifdef FULLSCREEN_EX
            // Disable click-through
            SetWindowLong(g_Variables.g_hCheatWindow, GWL_EXSTYLE,
                          WS_EX_TOPMOST | WS_EX_LAYERED);
#else
            SetWindowLong(g_Variables.g_hCheatWindow, GWL_EXSTYLE,
                          WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
#endif 
            SetForegroundWindow(g_Variables.g_hCheatWindow);
            SetFocus(g_Variables.g_hCheatWindow);
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            Logger::Log("OVERLAY", "Menu opened");
          }
        } else {
          g_MenuInfo.IsOpen = false;
          SetCursor(NULL);
          // Re-enable click-through
#ifdef FULLSCREEN_EX
          SetWindowLong(g_Variables.g_hCheatWindow, GWL_EXSTYLE,
                        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
#else
          SetWindowLong(g_Variables.g_hCheatWindow, GWL_EXSTYLE,
                        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW |
                            WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
#endif
          SetForegroundWindow(g_Variables.g_hGameWindow);
          SetFocus(g_Variables.g_hGameWindow);
          Logger::Log("OVERLAY", "Menu closed");
        }
      }

      static float BgAlpha = 0.f;
      BgAlpha = ImLerp(BgAlpha, g_MenuInfo.IsOpen ? 1.f : 0.f,
                       ImGui::GetIO().DeltaTime * 8);

      ImGui_ImplDX11_NewFrame();
      ImGui_ImplWin32_NewFrame();

      ImGui::NewFrame();
      {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, BgAlpha);

        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(0, 0),
            ImVec2(g_Variables.g_vGameWindowSize.x,
                   g_Variables.g_vGameWindowSize.y),
            ImColor(0.f, 0.f, 0.f, BgAlpha >= 0.4f ? 0.4f : BgAlpha));

        core::Features::g_Esp.Draw();

        if (g_Config.Aimbot->Enabled && g_Config.Aimbot->ShowFov) {
            ImGui::GetBackgroundDrawList()->AddCircle(
                g_Variables.g_vGameWindowCenter,
                (float)g_Config.Aimbot->FOV,
                g_Config.Aimbot->FovColor,
                100,
                1.0f
            );
        }

        Gui::Rendering();

        ImGui::PopStyleVar();
      }
      ImGui::EndFrame();

      const float ClearColor[4] = {0};
      g_pd3dDeviceContext->OMSetRenderTargets(1U, &g_mainRenderTargetView,
                                              NULL);
      g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView,
                                                 ClearColor);

      ImGui::Render();
      ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

      g_pSwapChain->Present(g_Config.General->VSync, 0U); // VSync

      frameCount++;
      if (frameCount == 1) {
        Logger::Log("OVERLAY", "First frame rendered successfully");
      }

    } catch (...) {
      Logger::Log("OVERLAY", "CRASH: C++ exception in render loop at frame " +
                                 std::to_string(frameCount));
      break;
    }
  }
  Logger::Log("OVERLAY", "Render loop exited. Frames rendered: " +
                             std::to_string(frameCount));

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  DestroyWindow(g_Variables.g_hCheatWindow);
  UnregisterClassW(wc.lpszClassName, wc.hInstance);

#ifdef FULLSCREEN_EX
  UnhookWindowsHookEx(hKeyboardHook);
#endif // FULLSCREEN_EX
  return;
}

void Overlay::Fonts() {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  io.IniFilename = nullptr;
  ImFontConfig cfg;
  cfg.SizePixels = 24;

  ImFont *defaultFont = io.Fonts->AddFontFromMemoryTTF(
      &anta_regular_hex, sizeof(anta_regular_hex), 20.f, &cfg,
      io.Fonts->GetGlyphRangesCyrillic());

  g_Variables.m_FontBig = defaultFont;
  g_Variables.m_FontBigSmall = defaultFont;
  g_Variables.m_FontNormal = defaultFont;
  g_Variables.m_FontSecundary = defaultFont;
  g_Variables.m_FontSmaller = defaultFont;
  g_Variables.m_DrawFont = defaultFont;
  g_Variables.FontAwesomeSolid = defaultFont;
  g_Variables.FontAwesomeSolidSmall = defaultFont;
  g_Variables.FontAwesomeRegular = defaultFont;
  g_Variables.m_Expand = defaultFont;
}

void Overlay::Style() {
  ImGui::StyleColorsDark();

  ImGuiStyle *style = &ImGui::GetStyle();
  {
    style->Alpha = 1.0f;
    style->WindowPadding = ImVec2(12, 12);
    style->WindowMinSize = ImVec2(32, 32);
    style->WindowRounding = 8.0f;
    style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style->ChildRounding = 6.0f;
    style->FramePadding = ImVec2(8, 6);
    style->FrameRounding = 6.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(6, 6);
    style->TouchExtraPadding = ImVec2(0, 0);
    style->IndentSpacing = 24.0f;
    style->ColumnsMinSpacing = 4.0f;
    style->ScrollbarSize = 14.0f;
    style->ScrollbarRounding = 8.0f;
    style->GrabMinSize = 8.0f;
    style->GrabRounding = 6.0f;
    style->ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style->DisplayWindowPadding = ImVec2(24, 24);
    style->DisplaySafeAreaPadding = ImVec2(6, 6);
    style->AntiAliasedLines = true;
    style->AntiAliasedFill = true;
    style->CurveTessellationTol = 1.25f;

    ImVec4 *colors = style->Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border] =
        ImVec4(1.00f, 0.40f, 0.70f, 0.20f); // Soft pink border
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] =
        ImVec4(1.00f, 0.40f, 0.70f, 0.40f); // Pink hover
    colors[ImGuiCol_FrameBgActive] =
        ImVec4(1.00f, 0.40f, 0.70f, 0.60f); // Pink active
    colors[ImGuiCol_TitleBg] =
        ImVec4(1.00f, 0.40f, 0.70f, 1.00f); // Pink title bar
    colors[ImGuiCol_TitleBgActive] =
        ImVec4(1.00f, 0.20f, 0.60f, 1.00f); // Brighter pink title
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.40f, 0.70f, 0.50f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] =
        ImVec4(1.00f, 0.40f, 0.70f, 1.00f); // Pink scrollbar
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.20f, 0.60f, 1.00f);
    colors[ImGuiCol_CheckMark] =
        ImVec4(1.00f, 0.40f, 0.70f, 1.00f); // Pink checkmark
    colors[ImGuiCol_SliderGrab] =
        ImVec4(1.00f, 0.40f, 0.70f, 1.00f); // Pink slider
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.20f, 0.60f, 1.00f);
    colors[ImGuiCol_Button] =
        ImVec4(1.00f, 0.40f, 0.70f, 0.80f); // Pink buttons
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.20f, 0.60f, 1.00f);
    colors[ImGuiCol_Header] =
        ImVec4(1.00f, 0.40f, 0.70f, 0.80f); // Pink headers (tabs)
    colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.20f, 0.60f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.40f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.40f, 0.70f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.40f, 0.70f, 0.80f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.20f, 0.60f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.50f, 0.80f, 0.80f);
    colors[ImGuiCol_TabActive] =
        ImVec4(1.00f, 0.40f, 0.70f, 1.00f); // Pink active tabs
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(1.00f, 0.40f, 0.70f, 0.50f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.40f, 0.70f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.40f, 0.70f, 1.00f);
  }

  if (g_Variables.Logo == nullptr) {
  }
}

bool Overlay::CreateDeviceD3D(HWND hWnd) {
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  if (D3D11CreateDeviceAndSwapChain(
          NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
          featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
          &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
    return false;

  CreateRenderTarget();
  return true;
}

void Overlay::CleanupDeviceD3D() {
  CleanupRenderTarget();

  if (g_pSwapChain) {
    g_pSwapChain->Release();
    g_pSwapChain = NULL;
  }
  if (g_pd3dDeviceContext) {
    g_pd3dDeviceContext->Release();
    g_pd3dDeviceContext = NULL;
  }
  if (g_pd3dDevice) {
    g_pd3dDevice->Release();
    g_pd3dDevice = NULL;
  }
}

void Overlay::CreateRenderTarget() {
  ID3D11Texture2D *pBackBuffer;
  g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL,
                                       &g_mainRenderTargetView);
  pBackBuffer->Release();
}

void Overlay::CleanupRenderTarget() {
  if (g_mainRenderTargetView) {
    g_mainRenderTargetView->Release();
    g_mainRenderTargetView = NULL;
  }
}

LRESULT CALLBACK Overlay::WndProc(HWND hWnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam) {

  if (msg == WM_SETCURSOR && !g_MenuInfo.IsOpen) {
    SetCursor(NULL);
    return TRUE;
  }

  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    return true;

  switch (msg) {
  case WM_SETCURSOR:
    if (g_MenuInfo.IsOpen) {
      SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    return TRUE;
  case WM_SIZE:
    if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
      Overlay::CleanupRenderTarget();
      g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam),
                                  DXGI_FORMAT_UNKNOWN, 0);
      Overlay::CreateRenderTarget();
    }
    return 0;
  case WM_SYSCOMMAND:
    if ((wParam & 0xfff0) == SC_KEYMENU)
      return 0;
    break;
  case WM_SYSCHAR:
    return 0;
  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
    return 0;
  case WM_DESTROY:
    ::PostQuitMessage(0);
    return 0;
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

DWORD Overlay::GetWinLogonToken(DWORD dwSessionId, DWORD dwDesiredAccess,
                                PHANDLE phToken) {
  DWORD dwErr;
  PRIVILEGE_SET ps;

  ps.PrivilegeCount = 1;
  ps.Control = PRIVILEGE_SET_ALL_NECESSARY;

  if (LookupPrivilegeValue(NULL, SE_TCB_NAME, &ps.Privilege[0].Luid)) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE != hSnapshot) {
      BOOL bCont, bFound = FALSE;
      PROCESSENTRY32 pe;

      pe.dwSize = sizeof(pe);
      dwErr = ERROR_NOT_FOUND;

      for (bCont = Process32First(hSnapshot, &pe); bCont;
           bCont = Process32Next(hSnapshot, &pe)) {
        HANDLE hProcess;

        if (0 != _tcsicmp(pe.szExeFile, xorstr("winlogon.exe"))) {
          continue;
        }

        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                               pe.th32ProcessID);
        if (hProcess) {
          HANDLE hToken;
          DWORD dwRetLen, sid;

          if (OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_DUPLICATE,
                               &hToken)) {
            BOOL fTcb;

            if (PrivilegeCheck(hToken, &ps, &fTcb) && fTcb) {
              if (GetTokenInformation(hToken, TokenSessionId, &sid, sizeof(sid),
                                      &dwRetLen) &&
                  sid == dwSessionId) {
                bFound = TRUE;
                if (DuplicateTokenEx(hToken, dwDesiredAccess, NULL,
                                     SecurityImpersonation, TokenImpersonation,
                                     phToken)) {
                  dwErr = ERROR_SUCCESS;
                } else {
                  dwErr = GetLastError();
                }
              }
            }
            CloseHandle(hToken);
          }
          CloseHandle(hProcess);
        }

        if (bFound)
          break;
      }

      CloseHandle(hSnapshot);
    } else {
      dwErr = GetLastError();
    }
  } else {
    dwErr = GetLastError();
  }

  return dwErr;
}

DWORD Overlay::CreateUIAccessToken(PHANDLE phToken) {
  DWORD dwErr;
  HANDLE hTokenSelf;

  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE,
                       &hTokenSelf)) {
    DWORD dwSessionId, dwRetLen;

    if (GetTokenInformation(hTokenSelf, TokenSessionId, &dwSessionId,
                            sizeof(dwSessionId), &dwRetLen)) {
      HANDLE hTokenSystem;

      dwErr = GetWinLogonToken(dwSessionId, TOKEN_IMPERSONATE, &hTokenSystem);
      if (ERROR_SUCCESS == dwErr) {
        if (SetThreadToken(NULL, hTokenSystem)) {
          if (DuplicateTokenEx(hTokenSelf,
                               TOKEN_QUERY | TOKEN_DUPLICATE |
                                   TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_DEFAULT,
                               NULL, SecurityAnonymous, TokenPrimary,
                               phToken)) {
            BOOL bUIAccess = TRUE;

            if (!SetTokenInformation(*phToken, TokenUIAccess, &bUIAccess,
                                     sizeof(bUIAccess))) {
              dwErr = GetLastError();
              CloseHandle(*phToken);
            }
          } else {
            dwErr = GetLastError();
          }
          RevertToSelf();
        } else {
          dwErr = GetLastError();
        }
        CloseHandle(hTokenSystem);
      }
    } else {
      dwErr = GetLastError();
    }

    CloseHandle(hTokenSelf);
  } else {
    dwErr = GetLastError();
  }

  return dwErr;
}

BOOL Overlay::CheckForUIAccess(DWORD *pdwErr, DWORD *pfUIAccess) {
  BOOL result = FALSE;
  HANDLE hToken;

  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
    DWORD dwRetLen;

    if (GetTokenInformation(hToken, TokenUIAccess, pfUIAccess,
                            sizeof(*pfUIAccess), &dwRetLen)) {
      result = TRUE;
    } else {
      *pdwErr = GetLastError();
    }
    CloseHandle(hToken);
  } else {
    *pdwErr = GetLastError();
  }

  return result;
}

DWORD Overlay::PrepareForUIAccess() {
  DWORD dwErr;
  HANDLE hTokenUIAccess;
  DWORD fUIAccess;

  if (CheckForUIAccess(&dwErr, &fUIAccess)) {
    if (fUIAccess) {
      dwErr = ERROR_SUCCESS;
    } else {
      dwErr = CreateUIAccessToken(&hTokenUIAccess);
      if (ERROR_SUCCESS == dwErr) {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        GetStartupInfo(&si);
        if (CreateProcessAsUser(hTokenUIAccess, NULL, GetCommandLine(), NULL,
                                NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
          CloseHandle(pi.hProcess), CloseHandle(pi.hThread);
          ExitProcess(0);
        } else {
          dwErr = GetLastError();
        }

        CloseHandle(hTokenUIAccess);
      }
    }
  }

  return dwErr;
}

} // namespace Gui