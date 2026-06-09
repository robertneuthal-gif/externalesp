#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace Logger {

    inline std::mutex g_LogMutex;
    inline std::string g_LogPath;

    inline void Init() {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        std::string dir(path);
        size_t pos = dir.find_last_of("\\/");
        if (pos != std::string::npos) dir = dir.substr(0, pos);
        g_LogPath = dir + "\\upx_log.txt";

        // Clear old log on start
        std::ofstream ofs(g_LogPath, std::ios::trunc);
        if (ofs.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto t = std::chrono::system_clock::to_time_t(now);
            struct tm tm_buf;
            localtime_s(&tm_buf, &t);
            ofs << "[" << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << "] UPX FiveM External - Log Started" << std::endl;
            ofs.close();
        }
    }

    inline void Log(const std::string& msg) {
        std::lock_guard<std::mutex> lock(g_LogMutex);
        std::ofstream ofs(g_LogPath, std::ios::app);
        if (ofs.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto t = std::chrono::system_clock::to_time_t(now);
            struct tm tm_buf;
            localtime_s(&tm_buf, &t);
            ofs << "[" << std::put_time(&tm_buf, "%H:%M:%S") << "] " << msg << std::endl;
            ofs.close();
        }
    }

    inline void Log(const std::string& category, const std::string& msg) {
        Log("[" + category + "] " + msg);
    }
}
