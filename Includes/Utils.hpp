#pragma once

#include <Includes/Includes.hpp>
#include <shlobj_core.h>
#include <fstream>
#include <sddl.h>
#include <random>
#include <regex>

namespace Utils {

	inline ImVec2 CalcTextSize( ImFont * font, int size, const char * label ) {
		return font->CalcTextSizeA( size, FLT_MAX, 0, label );
	}

	inline void OpenConsole( ) {
		AllocConsole( );
		FILE * fDummy;
		freopen_s( &fDummy, xorstr("CONIN$"), xorstr("r"), stdin );
		freopen_s( &fDummy, xorstr("CONOUT$"), xorstr("w"), stderr );
		freopen_s( &fDummy, xorstr("CONOUT$"), xorstr("w"), stdout );
	}

	inline std::string StringToLowerCase( std::string Input )
	{
		std::transform( Input.begin( ), Input.end( ), Input.begin( ), [ ] ( unsigned char c ) { return std::tolower( c ); } );
		return Input;
	}

	inline std::string StringToFirstUpperCase( std::string Input )
	{
		if ( !Input.empty( ) )
		{
			Input[ 0 ] = std::toupper( Input[ 0 ] );
			std::transform( Input.begin( ) + 1, Input.end( ), Input.begin( ) + 1, [ ] ( unsigned char c ) { return std::tolower( c ); } );
		}
		return Input;
	}

	inline static std::string EncodeB64( const std::string & in ) {

		std::string out;

		int val = 0, valb = -6;
		for ( u_char c : in ) {
			val = ( val << 8 ) + c;
			valb += 8;
			while ( valb >= 0 ) {
				out.push_back( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[ ( val >> valb ) & 0x3F ] );
				valb -= 6;
			}
		}
		if ( valb > -6 ) out.push_back( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[ ( ( val << 8 ) >> ( valb + 8 ) ) & 0x3F ] );
		while ( out.size( ) % 4 ) out.push_back( '=' );
		return out;
	}

	inline static std::string DecodeB64( const std::string & in ) {

		std::string out;

		std::vector<int> T( 256, -1 );
		for ( int i = 0; i < 64; i++ ) {
			T[ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[ i ] ] = i;
		}

		int val = 0, valb = -8;
		for ( u_char c : in ) {
			if ( T[ c ] == -1 ) break;
			val = ( val << 6 ) + T[ c ];
			valb += 6;
			if ( valb >= 0 ) {
				out.push_back( char( ( val >> valb ) & 0xFF ) );
				valb -= 8;
			}
		}
		return out;
	}

	inline std::string Hex2Str( const std::string & in ) {
		std::string output;

		size_t cnt = in.length( ) / 2;

		for ( size_t i = 0; cnt > i; ++i ) {
			uint32_t s = 0;
			std::stringstream ss;
			ss << std::hex << in.substr( i * 2, 2 );
			ss >> s;

			output.push_back( static_cast< unsigned char >( s ) );
		}

		return output;
	}

	inline bool KeyPressedWithDelay(int key, int delay_ms) {
		static std::chrono::time_point<std::chrono::steady_clock> lastTime;
		auto now = std::chrono::steady_clock::now();

		if ((GetAsyncKeyState(key) & 0x8000)) {
			if (now - lastTime >= std::chrono::milliseconds(delay_ms)) {
				lastTime = now;
				return true;
			}
		}
		return false;
	}

	inline std::string Str2Hex( const std::string & input )
	{
		static const char hex_digits[ ] = "0123456789ABCDEF";

		std::string output;
		output.reserve( input.length( ) * 2 );
		for ( unsigned char c : input )
		{
			output.push_back( hex_digits[ c >> 4 ] );
			output.push_back( hex_digits[ c & 15 ] );
		}
		return output;
	}

	inline std::pair<ImVec2, ImVec2> GetWindowPosAndSize( HWND window ) {
		RECT rect;
		if ( GetWindowRect( window, &rect ) ) {
			int x = rect.left;
			int y = rect.top;

			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			DWORD dwStyle = GetWindowLongPtr( window, GWL_STYLE );
			if ( dwStyle & WS_BORDER ) {
				x += 8; 
				y += 32;
				width -= 16;
				height -= 39;
			}

			ImVec2 position { ( float ) x, ( float ) y };
			ImVec2 size { ( float ) width, ( float ) height };
			return std::make_pair( position, size );
		}
		else {
			return std::make_pair( ImVec2 { 0, 0 }, ImVec2 { 0, 0 } );
		}
	}

	inline  std::string GetClipboard( ) {
		OpenClipboard( NULL );
		HANDLE hData = GetClipboardData( CF_TEXT );
		char * pszText = static_cast< char * >( GlobalLock( hData ) );
		std::string text( pszText );
		GlobalUnlock( hData );
		CloseClipboard( );
		return text;
	}

	inline void PasteClipboard( const char * seila ) {
		const size_t len = strlen( seila ) + 1;
		HGLOBAL hMem = GlobalAlloc( GMEM_MOVEABLE, len );
		memcpy( GlobalLock( hMem ), seila, len );
		GlobalUnlock( hMem );
		OpenClipboard( 0 );
		EmptyClipboard( );
		SetClipboardData( CF_TEXT, hMem );
		CloseClipboard( );
	}

	inline int GenRandomInt( int min, int max ) {
		std::random_device rd;
		std::mt19937 gen( rd( ) );
		std::uniform_int_distribution<> distrib( min, max );
		return distrib( gen );
	}

	inline void RenderBlur( HWND hwnd, bool remove )
	{
		struct ACCENTPOLICY {
			int na;
			int nf;
			int nc;
			int nA;
		};
		struct WINCOMPATTRDATA {
			int na;
			PVOID pd;
			ULONG ul;
		};

		const HINSTANCE hm = LoadLibrary( xorstr( "user32.dll" ) );
		if ( hm )
		{
			typedef BOOL( WINAPI * pSetWindowCompositionAttribute )( HWND, WINCOMPATTRDATA * );

			const pSetWindowCompositionAttribute SetWindowCompositionAttribute = ( pSetWindowCompositionAttribute ) GetProcAddress( hm, xorstr( "SetWindowCompositionAttribute" ) );
			if ( SetWindowCompositionAttribute )
			{
				ACCENTPOLICY policy;
				if ( remove ) {
					policy = { 0 };
				}
				else {
					policy = { 3, 0, 0, 0 };
				}
				WINCOMPATTRDATA data = { 19, &policy,sizeof( ACCENTPOLICY ) };

				SetWindowCompositionAttribute( hwnd, &data );
			}
			FreeLibrary( hm );
		}
	}

	inline std::string GetHWID( )
	{
		HANDLE hToken;
		if ( !OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY, &hToken ) ) {
			return "";
		}

		DWORD dwLength = 0;
		GetTokenInformation( hToken, TokenUser, nullptr, 0, &dwLength );
		if ( GetLastError( ) != ERROR_INSUFFICIENT_BUFFER ) {
			CloseHandle( hToken );
			return "";
		}

		std::vector<BYTE> buffer( dwLength );
		if ( !GetTokenInformation( hToken, TokenUser, buffer.data( ), dwLength, &dwLength ) ) {
			CloseHandle( hToken );
			return "";
		}

		CloseHandle( hToken );

		PTOKEN_USER pTokenUser = reinterpret_cast< PTOKEN_USER >( buffer.data( ) );

		LPSTR sid = nullptr;

		if ( !ConvertSidToStringSidA( pTokenUser->User.Sid, &sid ) ) {
			return "";
		}

		std::string sidStr( sid );

		LocalFree( sid );

		return sidStr;
	}
}