#pragma once
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <regex>


namespace core 
{
	namespace Threads 
	{
		class cScreenResolution 
		{
		public:
			void MoveAndSetResolution( )
			{
				g_Variables.g_hGameWindow = FindWindowA( xorstr( "grcWindow" ), nullptr );
				if ( !g_Variables.g_hGameWindow ) { exit( 0 ); }



				auto WindowInfo = Utils::GetWindowPosAndSize( g_Variables.g_hGameWindow );
				g_Variables.g_vGameWindowSize = WindowInfo.second;
				g_Variables.g_vGameWindowPos = WindowInfo.first;
				g_Variables.g_vGameWindowCenter = { g_Variables.g_vGameWindowSize.x / 2, g_Variables.g_vGameWindowSize.y / 2 };

				MoveWindow( g_Variables.g_hCheatWindow, g_Variables.g_vGameWindowPos.x, g_Variables.g_vGameWindowPos.y, g_Variables.g_vGameWindowSize.x, g_Variables.g_vGameWindowSize.y, true );
			}

			void Update( )
			{
				while ( true )
				{
					try
					{
						OutputDebugStringA("MoveAndSetResolution called\n");
						MoveAndSetResolution();
						OutputDebugStringA("MoveAndSetResolution finished\n");
					}
					catch (const std::exception& e)
					{
						MessageBoxA(NULL, e.what(), "Exception Caught", MB_ICONERROR);
						continue;
					}


					std::this_thread::sleep_for( std::chrono::milliseconds( 300 ) );
				}
			}
		};

		inline cScreenResolution g_ScreenResolution;
	}
}