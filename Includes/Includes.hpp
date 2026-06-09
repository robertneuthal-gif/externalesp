#pragma once

#define CURL_STATIC_LIB
#define IMGUI_DEFINE_MATH_OPERATORS

#define M_PI 3.14159265358979323846

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include <Security/xorstr.hpp>
#include <Security/Api/json.hpp>

#include <Security/Api/curl/curl.h>
#include <Globals.hpp>

#include <core/Config.hpp>

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include <d3dx11.h>
#include <d3d11.h>
#include <D3DX11tex.h>


#include <D3dx9math.h>

#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "Normaliz.lib" )
#pragma comment( lib, "Crypt32.lib" )
#pragma comment( lib, "Wldap32.lib" )
#pragma comment( lib, "libcurl.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "D3DX11.lib" )
#pragma comment( lib, "d3dx9.lib" )

#pragma warning( disable : 4996 )

using namespace core;