#pragma once
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <core/Offsets.hpp>
#include <core/core.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

namespace core
{
    namespace Threads
    {
        class cUpdatePtrs {
        public:
            void Update()
            {
                while (true)
                {
                  
                    core::sdk::Pointers::pWorld = driver.Read<CPedFactory*>(offsets.m_World);
                    core::sdk::Pointers::pLocalPlayer = core::sdk::Pointers::pWorld->GetLocalPlayer();
                    core::sdk::Pointers::pReplayInterFace = driver.Read<CReplayInterFace*>(offsets.m_ReplayInterFace);
                    core::sdk::Pointers::pViewPort = driver.Read<uintptr_t>(offsets.m_ViewPort);
                    core::sdk::Pointers::pCamGamePlayDirector = driver.Read<uintptr_t>(offsets.m_CamGameplayDirector);
 
                    std::this_thread::sleep_for(std::chrono::seconds(30));

                }
            }
        };

        inline cUpdatePtrs g_UpdatePtrs;
    }
}
