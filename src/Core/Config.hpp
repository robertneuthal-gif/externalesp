#pragma once
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <iostream>

namespace core {

	class Config {
	public:

		struct General {
		public:
			inline static bool StreamProof = true;
			inline static bool WaterMark;
			inline static bool WaterMarkCol;
			inline static bool ArrayList;
			inline static bool ArrayListCol;
			inline static bool VSync = true;
			inline static int ProcessPriority = 0;
			inline static int MenuKey = VK_INSERT;
		} *General;

		struct Aimbot {
		public:
			inline static bool Enabled = false;
			inline static bool ShowFov = true;
			inline static bool OnlyVisible = true;
			inline static bool IgnoreNPCs = true;
			inline static bool Prediction;
			inline static int FOV = 100;
			inline static int MaxDistance = 300;
			//inline static int SmoothHorizontal = 12;
			//inline static int SmoothVertical = 12;
			inline static int AimbotSpeed = 15;
			inline static int KeyBind = VK_RBUTTON;
			inline static int TargetBone = 0; // 0: Head, 1: Neck, 2: Pelvis
			inline static ImColor FovColor { 255, 255, 255 };
		} *Aimbot;

		struct TriggerBot {
		public:
			inline static bool Enabled;
			inline static bool ShowFov;
			inline static bool OnlyVisible;
			inline static bool IgnoreNPCs;
			inline static bool SmartTrigger;
			inline static int FOV = 20;
			inline static int MaxDistance = 200;
			inline static int Delay = 0;
			inline static int KeyBind;
			inline static ImColor FovColor { 255, 255, 255 };
		} *TriggerBot;

		struct SilentAim {
		public:
			inline static bool Enabled;
			inline static bool ShowFov;
			inline static bool OnlyVisible;
			inline static bool MagicBullets;
			inline static bool IgnoreNPCs;
			inline static int FOV = 40;
			inline static int MissChance = 0;
			inline static int HardMissChance = 0;
			inline static int MaxDistance = 200;
			inline static int KeyBind;
			inline static ImColor FovColor { 224, 94, 103, 200 };
		} *SilentAim;

		struct ESP {
		public:
			inline static bool UpdateCfgESP;
			inline static bool Enabled;
			inline static bool Box;
			inline static bool FilledBox;
			inline static int BoxState = 0;
			inline static bool p_skeleton;
			inline static bool HealthBar;
			inline static ImVec2 HealthBarPos;
			inline static int HealthBarState = 0;
			inline static bool ArmorBar;
			inline static ImVec2 ArmorBarPos;
			inline static int ArmorBarState = 0;
			inline static bool WeaponName;
			inline static ImVec2 WeaponNamePos;
			inline static int WeaponNameState = 0;
			inline static bool SnapLines;
			inline static bool UserNames;
			inline static ImVec2 UserNamesPos;
			inline static int UserNamesState = 0;
			inline static bool HeadCircle;
			inline static bool IgnoreNPCs;
			inline static bool ShowLocalPlayer;
			inline static bool HighlightVisible;
			inline static bool IgnoreDead;
			inline static bool DistanceFromMe;
			inline static bool FriendsMarker;
			inline static int FriendsMarkerBind;
			inline static ImVec2 DistanceFromMePos;
			inline static int DistanceFromMeState = 0;
			inline static int MaxDistance = 200;
			inline static float NameSize = 12.0f;
			inline static ImColor DistanceCol { 230, 230, 230, 255 };
			inline static ImColor UserNamesCol { 230, 230, 230, 255 };
			inline static ImColor WeaponNameCol { 230, 230, 230, 255 };
			inline static ImColor SkeletonCol { 255, 255, 255, 200 };
			inline static ImColor BoxCol { 255, 255, 255, 200 };
			inline static ImColor FilledBoxCol { 0, 0, 0, 40 };
			inline static ImColor SnapLinesCol { 255, 255, 255, 200 };
			inline static ImColor FriendCol { 255, 204, 0, 255 };
		} *ESP;

		struct VehicleESP {
		public:
			inline static bool Enabled;
			inline static bool SnapLines;
			inline static bool ShowLockUnlock;
			inline static bool VehName;
			inline static bool DistanceFromMe;
			inline static int MaxDistance = 200;
			inline static ImColor SnapLinesCol { 255, 255, 255, 200 };
		} *VehicleESP;

		struct Player {
		public:
			inline static float CurrentHealthValue;
			inline static float CurrentArmorValue;
			inline static bool FastRun;
			inline static float RunSpeed = 1.f;
			inline static bool InfiniteStamina;
			inline static bool WeaponOptions;
			inline static bool NoRecoilEnabled;
			inline static float RecoilValue;
			inline static bool NoSpreadEnabled;
			inline static float SpreadValue;
			inline static bool InfiniteAmmoEnabled;
			inline static bool NoReloadEnabled;
			inline static bool NoClipEnabled;
			inline static bool HandlingEditor;
			inline static int NoClipKey;
			inline static float NoClipSpeed = 2.0f;
			inline static bool InfiniteCombatRoll;
			inline static bool EnableGodMode;
			inline static bool VehicleGodMode;
			inline static bool SeatBelt;
			inline static bool ForceWeaponWheel;
			inline static bool ShrinkEnabled;
			inline static bool NoRagDollEnabled;
			inline static bool AntiHSEnabled;
			inline static bool StealCarEnabled;
			inline static int GodModeKey;
		} *Player;


		nlohmann::json ImColToJson( const ImColor & Col ) {
			return nlohmann::json::array( { Col.Value.x, Col.Value.y, Col.Value.z, Col.Value.w } );
		}

		ImColor JsonToImCol( const nlohmann::json & JsonCol ) {
			if ( JsonCol.is_array( ) && JsonCol.size( ) == 4 ) {
				float r = JsonCol[ 0 ];
				float g = JsonCol[ 1 ];
				float b = JsonCol[ 2 ];
				float a = JsonCol[ 3 ];
				return ImColor( r, g, b, a );
			}
			else {
				return ImColor( 0.0f, 0.0f, 0.0f, 1.0f );
			}
		}

		std::string SaveCurrentConfig( std::string CfgName )
		{
			try {
				nlohmann::json CfgJson;
				auto& GeneralCfg = CfgJson[ xorstr( "General" ) ];
				auto& FeaturesCfg = CfgJson;

				//General
				GeneralCfg[ xorstr( "StreamProof" ) ] = General->StreamProof;
				GeneralCfg[ xorstr( "WaterMark" ) ] = General->WaterMark;
				GeneralCfg[ xorstr( "ArrayList" ) ] = General->ArrayList;
				GeneralCfg[ xorstr( "VSync" ) ] = General->VSync;
				GeneralCfg[ xorstr( "ProcessPriority" ) ] = General->ProcessPriority;
				GeneralCfg[ xorstr( "MenuKey" ) ] = General->MenuKey;

				// Features - Aimbot
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "Enabled" ) ] = Aimbot->Enabled;
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "ShowFov" ) ] = Aimbot->ShowFov;
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "OnlyVisible" ) ] = Aimbot->OnlyVisible;
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "IgnoreNPCs" ) ] = Aimbot->IgnoreNPCs;
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "Prediction" ) ] = Aimbot->Prediction;
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "FOV" ) ] = Aimbot->FOV;
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "MaxDistance" ) ] = Aimbot->MaxDistance;
				//FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "SmoothHorizontal" ) ] = Aimbot->SmoothHorizontal;
				//FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "SmoothVertical" ) ] = Aimbot->SmoothHorizontal;
				FeaturesCfg[xorstr("Aimbot")][xorstr("AimSpeed")] = Aimbot->AimbotSpeed;
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "KeyBind" ) ] = Aimbot->KeyBind;
				FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "FovColor" ) ] = ImColToJson( Aimbot->KeyBind );

				// Features - TriggerBot
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "Enabled" ) ] = TriggerBot->Enabled;
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "ShowFov" ) ] = TriggerBot->ShowFov;
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "OnlyVisible" ) ] = TriggerBot->OnlyVisible;
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "IgnoreNPCs" ) ] = TriggerBot->IgnoreNPCs;
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "FOV" ) ] = TriggerBot->FOV;
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "MaxDistance" ) ] = TriggerBot->MaxDistance;
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "Delay" ) ] = TriggerBot->Delay;
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "KeyBind" ) ] = TriggerBot->KeyBind;
				FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "FovColor" ) ] = ImColToJson( TriggerBot->KeyBind );

				// Features - SilentAim
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "Enabled" ) ] = SilentAim->Enabled;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "ShowFov" ) ] = SilentAim->ShowFov;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "OnlyVisible" ) ] = SilentAim->OnlyVisible;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "IgnoreNPCs" ) ] = SilentAim->IgnoreNPCs;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "FOV" ) ] = SilentAim->FOV;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "MaxDistance" ) ] = SilentAim->MaxDistance;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "KeyBind" ) ] = SilentAim->KeyBind;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "MissChance" ) ] = SilentAim->MissChance;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "MagicBullets" ) ] = SilentAim->MagicBullets;
				FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "FovColor" ) ] = ImColToJson( SilentAim->KeyBind );

				// Features - ESP
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "Enabled" ) ] = ESP->Enabled;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "Box" ) ] = ESP->Box;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "BoxState" ) ] = ESP->BoxState;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "p_skeleton" ) ] = ESP->p_skeleton;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "HealthBar" ) ] = ESP->HealthBar;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "HealthBarState" ) ] = ESP->HealthBarState;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "ArmorBar" ) ] = ESP->ArmorBar;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "ArmorBarState" ) ] = ESP->ArmorBarState;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "WeaponName" ) ] = ESP->WeaponName;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "WeaponNameState" ) ] = ESP->WeaponNameState;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "SnapLines" ) ] = ESP->SnapLines;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "UserNames" ) ] = ESP->UserNames;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "UserNamesState" ) ] = ESP->UserNamesState;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "HeadCircle" ) ] = ESP->HeadCircle;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "IgnoreNPCs" ) ] = ESP->IgnoreNPCs;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "HighlightVisible" ) ] = ESP->HighlightVisible;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "IgnoreDead" ) ] = ESP->IgnoreDead;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "DistanceFromMe" ) ] = ESP->DistanceFromMe;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "DistanceFromMeState" ) ] = ESP->DistanceFromMeState;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "MaxDistance" ) ] = ESP->MaxDistance;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "ShowLocalPlayer" ) ] = ESP->ShowLocalPlayer;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "FriendsMarker" ) ] = ESP->FriendsMarker;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "FriendsMarkerBind" ) ] = ESP->FriendsMarkerBind;
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "DistanceCol" ) ] = ImColToJson( ESP->DistanceCol );
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "UserNamesCol" ) ] = ImColToJson( ESP->UserNamesCol );
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "WeaponNameCol" ) ] = ImColToJson( ESP->WeaponNameCol );
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "SkeletonCol" ) ] = ImColToJson( ESP->SkeletonCol );
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "BoxCol" ) ] = ImColToJson( ESP->BoxCol );
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "SnapLinesCol" ) ] = ImColToJson( ESP->SnapLinesCol );
				FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "FriendCol" ) ] = ImColToJson( ESP->FriendCol );

				// Features - VehicleESP
				FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "Enabled" ) ] = VehicleESP->Enabled;
				FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "SnapLines" ) ] = VehicleESP->SnapLines;
				FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "ShowLockUnlock" ) ] = VehicleESP->ShowLockUnlock;
				FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "VehName" ) ] = VehicleESP->VehName;
				FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "DistanceFromMe" ) ] = VehicleESP->DistanceFromMe;
				FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "MaxDistance" ) ] = VehicleESP->MaxDistance;
				FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "SnapLinesCol" ) ] = ImColToJson( VehicleESP->SnapLinesCol );

				// Features - Player
				FeaturesCfg[ xorstr( "Player" ) ][ xorstr( "NoClipKey" ) ] = Player->NoClipKey;
				FeaturesCfg[ xorstr( "Player" ) ][ xorstr( "NoClipSpeed" ) ] = Player->NoClipSpeed;
				FeaturesCfg[ xorstr( "Player" ) ][ xorstr( "GodModeKey" ) ] = Player->GodModeKey;

				std::string CfgJsonStr = CfgJson.dump( );

				Utils::PasteClipboard( Utils::EncodeB64( Utils::Str2Hex( Utils::EncodeB64( CfgJsonStr ) ) ).c_str( ) );

				return xorstr( "Config Exported to Clipboard." );
			}
			catch ( const std::exception & e ) {
				return xorstr( "Failed to save config." );
			}
		}

		std::string LoadCfg( std::string CfgName, std::string CfgCode )
		{
			try {
				std::string DecCfgCodeStr = Utils::DecodeB64( Utils::Hex2Str( Utils::DecodeB64( CfgCode ) ) );
				nlohmann::json CfgJson = nlohmann::json( ).parse( DecCfgCodeStr );

	
				auto& GeneralCfg = CfgJson[ xorstr( "General" ) ];
				auto& FeaturesCfg = CfgJson;

				// General
				if ( GeneralCfg != NULL )
				{
					General->StreamProof = GeneralCfg[ xorstr( "StreamProof" ) ];
					General->WaterMark = GeneralCfg[ xorstr( "WaterMark" ) ];
					General->ArrayList = GeneralCfg[ xorstr( "ArrayList" ) ];
					General->VSync = GeneralCfg[ xorstr( "VSync" ) ];
					General->ProcessPriority = GeneralCfg[ xorstr( "ProcessPriority" ) ];
					General->MenuKey = GeneralCfg[ xorstr( "MenuKey" ) ];
				}

				// Features - Aimbot
				Aimbot->Enabled = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "Enabled" ) ];
				Aimbot->ShowFov = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "ShowFov" ) ];
				Aimbot->OnlyVisible = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "OnlyVisible" ) ];
				Aimbot->IgnoreNPCs = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "IgnoreNPCs" ) ];
				Aimbot->Prediction = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "Prediction" ) ];
				Aimbot->FOV = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "FOV" ) ];
				Aimbot->MaxDistance = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "MaxDistance" ) ];
				//Aimbot->SmoothHorizontal = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "SmoothHorizontal" ) ];
				//Aimbot->SmoothVertical = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "SmoothVertical" ) ];
				Aimbot->AimbotSpeed = FeaturesCfg[xorstr("Aimbot")][xorstr("AimSpeed")];
				Aimbot->KeyBind = FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "KeyBind" ) ];
				Aimbot->FovColor = JsonToImCol( FeaturesCfg[ xorstr( "Aimbot" ) ][ xorstr( "FovColor" ) ] );

				// Features - TriggerBot
				TriggerBot->Enabled = FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "Enabled" ) ];
				TriggerBot->ShowFov = FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "ShowFov" ) ];
				TriggerBot->OnlyVisible = FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "OnlyVisible" ) ];
				TriggerBot->IgnoreNPCs = FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "IgnoreNPCs" ) ];
				TriggerBot->FOV = FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "FOV" ) ];
				TriggerBot->MaxDistance = FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "MaxDistance" ) ];
				TriggerBot->Delay = FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "Delay" ) ];
				TriggerBot->KeyBind = FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "KeyBind" ) ];
				TriggerBot->FovColor = JsonToImCol( FeaturesCfg[ xorstr( "TriggerBot" ) ][ xorstr( "FovColor" ) ] );

				// Features - SilentAim
				SilentAim->Enabled = FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "Enabled" ) ];
				SilentAim->ShowFov = FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "ShowFov" ) ];
				SilentAim->OnlyVisible = FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "OnlyVisible" ) ];
				SilentAim->IgnoreNPCs = FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "IgnoreNPCs" ) ];
				SilentAim->FOV = FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "FOV" ) ];
				SilentAim->MaxDistance = FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "MaxDistance" ) ];
				SilentAim->KeyBind = FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "KeyBind" ) ];
				SilentAim->MagicBullets = FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "MagicBullets" ) ];
				SilentAim->FovColor = JsonToImCol( FeaturesCfg[ xorstr( "SilentAim" ) ][ xorstr( "FovColor" ) ] );

				// Features - ESP
				ESP->Enabled = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "Enabled" ) ];
				ESP->Box = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "Box" ) ];
				ESP->BoxState = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "BoxState" ) ];
				ESP->p_skeleton = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "p_skeleton" ) ];
				ESP->HealthBar = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "HealthBar" ) ];
				ESP->HealthBarState = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "HealthBarState" ) ];
				ESP->ArmorBar = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "ArmorBar" ) ];
				ESP->ArmorBarState = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "ArmorBarState" ) ];
				ESP->WeaponName = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "WeaponName" ) ];
				ESP->WeaponNameState = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "WeaponNameState" ) ];
				ESP->SnapLines = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "SnapLines" ) ];
				ESP->UserNames = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "UserNames" ) ];
				ESP->UserNamesState = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "UserNamesState" ) ];
				ESP->HeadCircle = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "HeadCircle" ) ];
				ESP->IgnoreNPCs = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "IgnoreNPCs" ) ];
				ESP->HighlightVisible = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "HighlightVisible" ) ];
				ESP->IgnoreDead = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "IgnoreDead" ) ];
				ESP->DistanceFromMe = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "DistanceFromMe" ) ];
				ESP->DistanceFromMeState = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "DistanceFromMeState" ) ];
				ESP->ShowLocalPlayer = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "ShowLocalPlayer" ) ];
				ESP->MaxDistance = FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "MaxDistance" ) ];
				ESP->DistanceCol = JsonToImCol( FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "DistanceCol" ) ] );
				ESP->UserNamesCol = JsonToImCol( FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "UserNamesCol" ) ] );
				ESP->WeaponNameCol = JsonToImCol( FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "WeaponNameCol" ) ] );
				ESP->SkeletonCol = JsonToImCol( FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "SkeletonCol" ) ] );
				ESP->BoxCol = JsonToImCol( FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "BoxCol" ) ] );
				ESP->SnapLinesCol = JsonToImCol( FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "SnapLinesCol" ) ] );
				ESP->FriendCol = JsonToImCol( FeaturesCfg[ xorstr( "ESP" ) ][ xorstr( "FriendCol" ) ] );

				// Features - VehicleESP
				VehicleESP->Enabled = FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "Enabled" ) ];
				VehicleESP->SnapLines = FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "SnapLines" ) ];
				VehicleESP->ShowLockUnlock = FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "ShowLockUnlock" ) ];
				VehicleESP->VehName = FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "VehName" ) ];
				VehicleESP->DistanceFromMe = FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "DistanceFromMe" ) ];
				VehicleESP->MaxDistance = FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "MaxDistance" ) ];
				VehicleESP->SnapLinesCol = JsonToImCol( FeaturesCfg[ xorstr( "VehicleESP" ) ][ xorstr( "SnapLinesCol" ) ] );

				// Features - Player
				Player->NoClipKey = FeaturesCfg[ xorstr( "Player" ) ][ xorstr( "NoClipKey" ) ];
				Player->NoClipSpeed = FeaturesCfg[ xorstr( "Player" ) ][ xorstr( "NoClipSpeed" ) ];
				Player->GodModeKey = FeaturesCfg[ xorstr( "Player" ) ][ xorstr( "GodModeKey" ) ];

				ESP->UpdateCfgESP = true;

				return xorstr( "Config loaded with success." );
			}
			catch ( const std::exception & e ) {
				//std::cout << e.what() << "\n";
				return xorstr( "Error Loading Config!");
			}
		}

	};

	inline Config g_Config;

}