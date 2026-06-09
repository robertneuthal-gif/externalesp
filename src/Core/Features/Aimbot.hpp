#pragma once
#include <Includes/Includes.hpp>
#include <core/Offsets.hpp>
#include <core/sdk/sdk.hpp>

inline void PrintVector3( const D3DXVECTOR3 & vector ) {
	std::cout << vector.x << '\\' << vector.y << '\\' << vector.z << '\n';
}


namespace core
{
	namespace Features
	{
		class cAimbot 
		{
		private:
			void SetViewAngles( CPed * Ped, D3DXVECTOR3 BonePos );
		public:
			void Start( );
		};

		inline cAimbot g_Aimbot;
	}
}