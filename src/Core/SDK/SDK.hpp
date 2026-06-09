#pragma once
#include <Includes/Includes.hpp>
#include <core/sdk/Memory.hpp>
#include <core/Offsets.hpp>
#include <core/sdk/Structs/GameClasses.hpp>


#include <d3dx11.h>
#include <d3d11.h>
#include <D3DX11tex.h>
#include <D3dx9math.h>
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "D3DX11.lib" )

namespace core {

	namespace sdk {

		inline std::mutex skeletonMutex;

		namespace Pointers {
			// Object Pointers
			inline CPedFactory * pWorld = nullptr;
			inline CPed * pLocalPlayer = nullptr;
			inline CReplayInterFace * pReplayInterFace = nullptr;
			inline uintptr_t pBlipList = 0, pViewPort = 0, pCamGamePlayDirector = 0;
		}

		namespace game
		{
			struct cSkeleton_t {

				std::unordered_map<unsigned int, int> stored_bones_idx;

				struct crSkeletonData_t {
					uintptr_t Ptr, m_bone_indexTable;
					unsigned int m_Used; // crSkeletonData->m_bone_indexTable.m_Used
					unsigned int m_NumBones; // crSkeletonData->m_NumBones
					unsigned short m_bone_indexTable_Slots;
				} crSkeletonData;

				uintptr_t player_skeleton, Arg2;
				D3DXMATRIX Arg1;

				bool IsValid() const {
					return player_skeleton && crSkeletonData.Ptr && crSkeletonData.m_bone_indexTable_Slots;
				}



			};

			struct NetworkInfo {
				std::string UserName;
				std::string DiscordId;
				std::string SteamId;
			};

			struct EntityStruct {
				CPed * Ped;

				int Id;
				int Index;
				int PedType;

				bool IsFriend;

				bool IsVisible;

				core::sdk::game::cSkeleton_t Skeleton;


				D3DXVECTOR3 Pos;

				float Health;
				float MaxHealth;
				float Armor;
				float Distance;

				std::string WeaponName;

				NetworkInfo NetworkInfo;

				float HealthAnim;
			};

			struct EspAnim {
				bool CanFadeOut;
				float Health;
				float Armor;
				float Alpha;
			};

			struct VehicleStructure {
				CVehicle * Pointer;
				CPed * Driver;
				std::string Name;
				float Dist;
				bool IsLocked;
				D3DXVECTOR3 Pos;
			};



			inline std::vector<EntityStruct> EntityList;
			inline std::vector<VehicleStructure> VehicleList;
			inline std::unordered_map<CPed *, bool> FriendMap;

			inline D3DXVECTOR2 w2s( D3DXVECTOR3 World )
			{
				D3DXMATRIX ViewMatrix = driver.Read<D3DXMATRIX>( core::sdk::Pointers::pViewPort + 0x24C );
				D3DXMatrixTranspose( &ViewMatrix, &ViewMatrix );

				auto VecX = D3DXVECTOR4( ViewMatrix._21, ViewMatrix._22, ViewMatrix._23, ViewMatrix._24 ),
					VecY = D3DXVECTOR4( ViewMatrix._31, ViewMatrix._32, ViewMatrix._33, ViewMatrix._34 ),
					VecZ = D3DXVECTOR4( ViewMatrix._41, ViewMatrix._42, ViewMatrix._43, ViewMatrix._44 );

				D3DXVECTOR3 ScreenPos = D3DXVECTOR3(
					( VecX.x * World.x ) + ( VecX.y * World.y ) + ( VecX.z * World.z ) + VecX.w,
					( VecY.x * World.x ) + ( VecY.y * World.y ) + ( VecY.z * World.z ) + VecY.w,
					( VecZ.x * World.x ) + ( VecZ.y * World.y ) + ( VecZ.z * World.z ) + VecZ.w
				);

				if ( ScreenPos.z <= 0.1f )
					return D3DXVECTOR2( 0, 0 );

				ScreenPos.z = 1.0f / ScreenPos.z;
				ScreenPos.x *= ScreenPos.z;
				ScreenPos.y *= ScreenPos.z;

				ScreenPos.x += ( g_Variables.g_vGameWindowSize.x / 2 ) + float( 0.5f * ScreenPos.x * g_Variables.g_vGameWindowSize.x + 0.5f );
				ScreenPos.y = ( g_Variables.g_vGameWindowSize.y / 2 ) - float( 0.5f * ScreenPos.y * g_Variables.g_vGameWindowSize.y + 0.5f );
				return D3DXVECTOR2( ScreenPos.x, ScreenPos.y );
			}

			inline std::unordered_map<uintptr_t, std::unordered_map<unsigned int, int>> stored_bones_idx;
			inline std::mutex bones_cache;


			inline bool get_bone_id( cSkeleton_t cSkeleton, unsigned int bone_index, int & outbone_indexx )
			{
				std::lock_guard<std::mutex> lock( bones_cache );
				{
					auto stored_bone_id_cache = stored_bones_idx[ cSkeleton.crSkeletonData.Ptr ];

					if ( stored_bone_id_cache.find( bone_index ) != stored_bone_id_cache.end( ) ) {
						outbone_indexx = stored_bone_id_cache[ bone_index ];
						return true;
					}

					if ( cSkeleton.crSkeletonData.m_Used != 0 ) 
					{
						unsigned short m_bone_indexTable_Slots = cSkeleton.crSkeletonData.m_bone_indexTable_Slots; 

						std::uintptr_t m_bone_indexTable_Hash = driver.Read<std::uintptr_t>( cSkeleton.crSkeletonData.m_bone_indexTable + 0x8 * ( bone_index % ( unsigned int ) m_bone_indexTable_Slots ) );
						for ( std::uintptr_t i = m_bone_indexTable_Hash; i != 0; i = driver.Read<std::uintptr_t>( i + 0x8 ) )
						{
							int i_key = driver.Read<int>( i );
							if ( bone_index == i_key )
							{
								int p_Data = driver.Read<int>( i + 0x4 );
								if ( p_Data )
								{
									outbone_indexx = p_Data;
									stored_bone_id_cache[ bone_index ] = p_Data;
									return true;
								}
							}
						}
					}
					else if ( bone_index < cSkeleton.crSkeletonData.m_NumBones )
					{
						outbone_indexx = bone_index;
						stored_bone_id_cache[ bone_index ] = bone_index;

						return true;
					}

					return false;
				}
			}

			inline D3DXVECTOR3 get_bone_pos_inst_frag( std::uintptr_t InstFrag, unsigned int bone_index, D3DXMATRIX Arg1, uintptr_t Arg2 )
			{
				D3DXMATRIX v4;
				D3DXMATRIX Result; 

				v4 = Arg1;
				if ( !v4 ) { return D3DXVECTOR3( 0, 0, 0 ); }

				Result = driver.Read<D3DXMATRIX>( Arg2 + ( ( unsigned __int64 ) bone_index << 6 ) );
				if ( !Result ) { return D3DXVECTOR3( 0, 0, 0 ); }

				D3DXVECTOR3 vec1( v4._11, v4._12, v4._13 );
				D3DXVECTOR3 vec2( v4._21, v4._22, v4._23 );
				D3DXVECTOR3 vec3( v4._31, v4._32, v4._33 );
				D3DXVECTOR3 vec4( v4._41, v4._42, v4._43 );
				D3DXVECTOR3 vec5( Result._41, Result._42, Result._43 );
				return D3DXVECTOR3(
					vec1.x * vec5.x + vec4.x + vec2.x * vec5.y + vec3.x * vec5.z,
					vec1.y * vec5.x + vec4.y + vec2.y * vec5.y + vec3.y * vec5.z,
					vec1.z * vec5.x + vec4.z + vec2.z * vec5.y + vec3.z * vec5.z
				);
			}

			inline D3DXVECTOR3 get_bone_pos_r( CPed * Ped, unsigned int Mask, cSkeleton_t cSkeleton_t )
			{
				bool result = false;

				int bone_index = 0;

				if ( get_bone_id( cSkeleton_t, Mask, bone_index ) ) {
					return get_bone_pos_inst_frag( cSkeleton_t.player_skeleton, bone_index, cSkeleton_t.Arg1, cSkeleton_t.Arg2 );
				}

				return Ped->GetPos( );
			}

			inline std::string GetPedName( /*int id*/ CPed * Ped )
			{
				/*for (int i = 0; i < sdk::game::EntityList.size(); i++)
				{
					uintptr_t NetPointer = driver.Read<uintptr_t>(offsets.m_NetIdToNamesEntry + (i * 0x8));

					if (!NetPointer) continue;

					int Id = driver.Read<int>(NetPointer + 0x10);

					if (Id == id) {
						return driver.ReadString(NetPointer + 0x18);
					}
				}*/

				for ( auto Ent : EntityList )
				{
					if ( Ent.Ped != Ped )
						continue;

					return Ent.NetworkInfo.UserName;
				}

			}

			inline bool IsOnScreen( D3DXVECTOR2 Pos )
			{
				if ( Pos == D3DXVECTOR2( 0, 0 ) ) return false;
				return true;
			}

			inline CPed * GetClosestPed( int MaxDistance, bool IgnoreNpc, bool VisibleCheck, int MaxFOV = 1000 )
			{
				CPed * ClosestPed = nullptr;
				float ClosestDist = FLT_MAX;

				for ( auto & Entity : core::sdk::game::EntityList )
				{
					CPed * Ped = Entity.Ped;

					if ( Ped == core::sdk::Pointers::pLocalPlayer )
						continue;

					if ( std::abs( Entity.Health ) <= 101.f )
						continue;

					if ( IgnoreNpc && Entity.PedType != 2 )
						continue;

					if ( Entity.IsFriend )
						continue;

					if ( Entity.Distance > MaxDistance )
						continue;

					D3DXVECTOR2 PedPos = core::sdk::game::w2s( Entity.Pos );
					if ( !IsOnScreen( PedPos ) ) continue;

					float x = PedPos.x - g_Variables.g_vGameWindowCenter.x;
					float y = PedPos.y - g_Variables.g_vGameWindowCenter.y;
					float AimDist = std::sqrt( ( x * x ) + ( y * y ) );

					if ( AimDist > (float)MaxFOV )
						continue;

					if ( AimDist < ClosestDist )
					{
						ClosestDist = AimDist;
						ClosestPed = Ped;
					}
				}

				return ClosestPed;
			}

		}
	}

}