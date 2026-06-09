#pragma once
#include <Includes/Includes.hpp>
#include <core/Offsets.hpp>
#include <core/sdk/sdk.hpp>

#include <Gui/lib_draw/draw.h>

extern const char * keys[ ];

namespace core
{
	namespace Features
	{
		class cEsp
		{
		public:
			void Draw( );
			void DrawVehicle();

			void UpdateSkeletonsThread();
		};

		inline cEsp g_Esp;

	}
}
