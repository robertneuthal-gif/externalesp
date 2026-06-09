#include "Aimbot.hpp"
#include <cmath>
#include <algorithm>
#include <thread>

constexpr float PI = 3.14159265f;



void core::Features::cAimbot::SetViewAngles(CPed* Ped, D3DXVECTOR3 BonePos) {
    uintptr_t cam = driver.Read<uintptr_t>(core::sdk::Pointers::pCamGamePlayDirector + 0x2C0);
    if (!cam) return;

    D3DXVECTOR3 camPos = driver.Read<D3DXVECTOR3>(cam + 0x30);
    if (D3DXVec3Length(&camPos) < 1.0f) {
        camPos = core::sdk::Pointers::pLocalPlayer->GetPos();
        camPos.z += 0.8f;
    }

    D3DXVECTOR3 targetDir = BonePos - camPos;
    D3DXVec3Normalize(&targetDir, &targetDir);

    D3DXVECTOR3 currentDir = driver.Read<D3DXVECTOR3>(cam + 0x40);
    D3DXVec3Normalize(&currentDir, &currentDir);

    float speed = (float)g_Config.Aimbot->AimbotSpeed / 100.0f;
    if (speed > 1.0f) speed = 1.0f;

    D3DXVECTOR3 finalDir;
    D3DXVec3Lerp(&finalDir, &currentDir, &targetDir, speed);
    D3DXVec3Normalize(&finalDir, &finalDir);

    driver.Write<D3DXVECTOR3>(cam + 0x40, finalDir);
    driver.Write<D3DXVECTOR3>(cam + 0x3D0, finalDir);
}

void core::Features::cAimbot::Start() {
    while (true) {
        if (g_Config.Aimbot->Enabled && g_Config.Aimbot->KeyBind && (GetAsyncKeyState(g_Config.Aimbot->KeyBind) & 0x8000)
            && GetForegroundWindow() != g_Variables.g_hCheatWindow) {

            CPed* Ped = core::sdk::game::GetClosestPed(g_Config.Aimbot->MaxDistance, g_Config.Aimbot->IgnoreNPCs, g_Config.Aimbot->OnlyVisible, g_Config.Aimbot->FOV);
            if (!Ped) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            core::sdk::game::cSkeleton_t skeleton;
            uintptr_t FragInstNMGta = driver.Read<uintptr_t>((uintptr_t)Ped + offsets.m_FragInst);
            if (!FragInstNMGta) goto end_loop;

            uintptr_t v9 = driver.Read<uintptr_t>(FragInstNMGta + 0x68);
            if (!v9) goto end_loop;

            skeleton.player_skeleton = driver.Read<uintptr_t>(v9 + 0x178);
            if (!skeleton.player_skeleton) goto end_loop;

            skeleton.crSkeletonData.Ptr = driver.Read<uintptr_t>(skeleton.player_skeleton);
            if (!skeleton.crSkeletonData.Ptr) goto end_loop;

            skeleton.crSkeletonData.m_Used = driver.Read<unsigned int>(skeleton.crSkeletonData.Ptr + 0x1A);
            skeleton.crSkeletonData.m_NumBones = driver.Read<unsigned int>(skeleton.crSkeletonData.Ptr + 0x5E);
            skeleton.crSkeletonData.m_bone_indexTable_Slots = driver.Read<unsigned short>(skeleton.crSkeletonData.Ptr + 0x18);
            skeleton.crSkeletonData.m_bone_indexTable = driver.Read<uintptr_t>(skeleton.crSkeletonData.Ptr + 0x10);
            
            skeleton.Arg1 = driver.Read<D3DXMATRIX>(driver.Read<uintptr_t>(skeleton.player_skeleton + 0x8));
            skeleton.Arg2 = driver.Read<uintptr_t>(skeleton.player_skeleton + 0x18);

            int targetBoneMask = SKEL_Head;
            switch (g_Config.Aimbot->TargetBone) {
                case 1: targetBoneMask = SKEL_neck_1; break;
                case 2: targetBoneMask = SKEL_pelvis; break;
                default: targetBoneMask = SKEL_Head; break;
            }

            D3DXVECTOR3 TargetPos = core::sdk::game::get_bone_pos_r(Ped, targetBoneMask, skeleton);
            if (TargetPos != D3DXVECTOR3(0, 0, 0)) {
                SetViewAngles(Ped, TargetPos);
            }
        }

    end_loop:
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
