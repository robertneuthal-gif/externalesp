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
        class cVehicleList {
        private:
            CVehInterFace * VehInterface = nullptr;
            CVehicleList * VehList = nullptr;
            int MaxVehicles;
        public:
            std::mutex vehicleListMutex;
        public:
            void Update( )
            {
              
            }
        };

        inline cVehicleList g_VehicleList;
    }
}