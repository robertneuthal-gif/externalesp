#include "Player.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <wtypes.h>
#include <Psapi.h>

const std::wstring g_SpotifyPrefix = L"";
const std::wstring g_SpotifyDelimiter = L" - ";

// Static members initialization
std::wstring Spotify::g_cachedAuthor = L"";
std::wstring Spotify::g_cachedTitle = L"";
std::atomic<bool> Spotify::g_isPlaying(false);
static HWND mWnd = NULL;

Spotify::Spotify() : mAuthor(L""), mTitle(L"") {
  update();
}

Spotify::~Spotify() {}


bool Spotify::update() {
  bool changed = false;
  updateWindow();
  updateTitle(changed);  // Ensure title is checked for changes

  // If anything has changed, update the global variables
  if (changed) {
    g_cachedAuthor = mAuthor;  // Use local cached values to update globals
    g_cachedTitle = mTitle;
  
  }

  return changed;
}

const std::wstring& Spotify::author() const {
  return g_cachedAuthor;
}

const std::wstring& Spotify::title() const {
  return g_cachedTitle;
}

bool Spotify::isPlaying() const {
  return g_isPlaying.load();
}

void Spotify::updateWindow() {
  if (!mWnd || !IsWindow(mWnd)) {
    mWnd = NULL;

    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
      // Check if window is visible and enabled
      if (!IsWindowVisible(hwnd) || !IsWindowEnabled(hwnd))
        return TRUE;

      // Ignore minimized windows
      if (IsIconic(hwnd))
        return TRUE;

      DWORD processId = 0;
      GetWindowThreadProcessId(hwnd, &processId);

      if (processId == 0)
        return TRUE;

      HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
      if (hProcess) {
        wchar_t processName[MAX_PATH] = L"<unknown>";
        GetModuleBaseNameW(hProcess, NULL, processName, MAX_PATH);

        // Make sure it's Spotify.exe
        if (_wcsicmp(processName, L"Spotify.exe") == 0) {
          *reinterpret_cast<HWND*>(lParam) = hwnd;
          CloseHandle(hProcess);
          return FALSE; // Found it, stop enumerating
        }
        CloseHandle(hProcess);
      }

      return TRUE; // Keep searching
      }, reinterpret_cast<LPARAM>(&mWnd));
  }
}


void Spotify::updateString(std::wstring& str, const std::wstring& newStr, bool& changed) {
  changed |= (str != newStr);
  if (changed) {
    str = newStr;
  }
}

void Spotify::updateTitle(bool& changed) {
  if (!mWnd || !IsWindow(mWnd)) return;

  int len = GetWindowTextLengthW(mWnd);
  if (len > 0) {
    std::wstring tmp(len, L'\0');
    GetWindowTextW(mWnd, tmp.data(), len + 1);

    // Determine the start position of the song title
    std::wstring::size_type start = tmp.compare(0, g_SpotifyPrefix.length(), g_SpotifyPrefix) == 0
      ? g_SpotifyPrefix.length() : 0;

    // Find the delimiter " - " and separate the author and title
    std::wstring::size_type endp = tmp.find(g_SpotifyDelimiter, start);

    // Check if the author has changed
    updateString(mAuthor, tmp.substr(start, (endp != std::wstring::npos) ? (endp - start) : endp), changed);

    // Update title based on the position of the delimiter
    if (endp == std::wstring::npos) {
      updateString(mTitle, L"", changed);  // No title part
    }
    else {
      start = endp + g_SpotifyDelimiter.length();
      updateString(mTitle, tmp.substr(start), changed);  // The title is after the delimiter
    }

    // Check if the window title indicates playing status (Spotify Free, paused, etc.)
    if (tmp.find(L"Spotify Free") != std::wstring::npos || tmp.find(L"Spotify") != std::wstring::npos) {
      g_isPlaying = false;  // If it's a Spotify window and we find certain keywords, assume playing
    }
    else {
      g_isPlaying = true; // Otherwise, it's considered paused or stopped
    }
  }
  else {
    // If there is no window text, reset both fields
    updateString(mAuthor, L"", changed);
    updateString(mTitle, L"", changed);
    g_isPlaying = false; // Set as not playing if no window is found
  }
}





void Spotify::startSpotifyUpdateThread() {
  spotifyUpdateThread();
}

void Spotify::spotifyUpdateThread() {
  Spotify spotify;

  while (true) {
    bool updated = spotify.update();  // Check for updates

    if (updated) {
      // If something has changed, update the cached author and title
      g_cachedAuthor = spotify.author();
      g_cachedTitle = spotify.title();
      g_isPlaying = spotify.isPlaying();


    }

    // Sleep for 1 second before checking again
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
