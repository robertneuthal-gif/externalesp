#include <Includes/Includes.hpp>

namespace Gui {

	static ID3D11Device * g_pd3dDevice;
	static ID3D11DeviceContext * g_pd3dDeviceContext;
	static IDXGISwapChain * g_pSwapChain;
	static UINT                     g_ResizeWidth, g_ResizeHeight;
	static ID3D11RenderTargetView * g_mainRenderTargetView;
	inline MSG Message = { NULL };

	class Overlay {
	private:
		bool CreateDeviceD3D( HWND hWnd );
		void CleanupDeviceD3D( );
		static void CreateRenderTarget( );
		static void CleanupRenderTarget( );
		void LoadWeaponIcons( );
		static LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

		DWORD PrepareForUIAccess( );
		static BOOL CheckForUIAccess( DWORD * pdwErr, DWORD * pfUIAccess );
		static DWORD CreateUIAccessToken( PHANDLE phToken );
		static DWORD GetWinLogonToken( DWORD dwSessionId, DWORD dwDesiredAccess, PHANDLE phToken );

		static void move_window(HWND hWnd);
		static bool full_screen(HWND windowHandle);


		enum ZBID {
			ZBID_DEFAULT = 0,
			ZBID_DESKTOP = 1,
			ZBID_UIACCESS = 2,
			ZBID_IMMERSIVE_IHM = 3,
			ZBID_IMMERSIVE_NOTIFICATION = 4,
			ZBID_IMMERSIVE_APPCHROME = 5,
			ZBID_IMMERSIVE_MOGO = 6,
			ZBID_IMMERSIVE_EDGY = 7,
			ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
			ZBID_IMMERSIVE_INACTIVEDOCK = 9,
			ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
			ZBID_IMMERSIVE_ACTIVEDOCK = 11,
			ZBID_IMMERSIVE_BACKGROUND = 12,
			ZBID_IMMERSIVE_SEARCH = 13,
			ZBID_GENUINE_WINDOWS = 14,
			ZBID_IMMERSIVE_RESTRICTED = 15,
			ZBID_SYSTEM_TOOLS = 16,
			// Win10
			ZBID_LOCK = 17,
			ZBID_ABOVELOCK_UX = 18,
		};

		D3DX11_IMAGE_LOAD_INFO Info; 
		ID3DX11ThreadPump * Pump { nullptr };

	public:
		void Render( );
		void Style( );
		void Fonts( );
	};

	inline Overlay cOverlay;

}