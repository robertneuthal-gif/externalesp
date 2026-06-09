#pragma once

namespace core {

    class OffsetsClass {
    public:

        //Class Addrs
        uintptr_t m_World, //48 8B 05 ? ? ? ? 33 D2 48 8B 40 08 8A CA 48 85 C0 74 16 48 8B
            m_ReplayInterFace, //48 8D 0D ?? ?? ?? ?? 48 ?? ?? E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? 8A D8 E8 ?? ?? ?? ?? 84 DB 75 13 48 8D 0D ?? ?? ?? ?? 48 8B D7 E8 ?? ?? ?? ?? 84 C0 74 BC 8B 8F
            m_ViewPort, //48 8B 15 ? ? ? ? 48 8D 2D ? ? ? ? 48 8B CD
            m_BlipList, //4C 8D 05 ? ? ? ? 0F B7 C1
            m_CamGameplayDirector, //4C 8B 35 ? ? ? ? 33 FF 32 D            
            m_IsVisiblePed,
            m_LocalPlayer;


        //Function Addrs
        uintptr_t m_SilentAim,
            m_InfiniteAmmo0,
            m_InfiniteAmmo1,
            m_MagicBulletsPatch,
            m_ArmsKinematics,
            m_LegsKinematics,
            m_InfiniteCombatRoll,
            m_GiveWeapon;

        //Offsets
        uintptr_t m_LastVehicle,
            m_Handling,
            m_VehicleEngineHealth,
            m_PlayerInfo,
            m_FragInst,
            m_Armor,
            m_Recoil,
            m_PedFlag,
            m_Spread,
            m_WeaponManager,
            m_WeaponInfo,
            m_VehicleDoorsLockState,
            m_EntityType,
            m_Speed,
            m_MaxHealth,
            m_VehicleDriver,
            m_VehicleGravity,
            m_CObject,
            m_NoRagDoll,
            m_CWeapon,
            m_PlayerId,
            m_CitizenNamesModBase,
            m_NetIdToNamesEntry,
            m_SeatBealt,
            m_AimCPedPatternResult;
           

        //Infos
        uint32_t CurrentBuild;
        std::string ServerIp;

        

    };

    inline OffsetsClass offsets;
}