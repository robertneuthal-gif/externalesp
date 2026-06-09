#pragma once

#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <core/Offsets.hpp>
#include <core/core.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <d3dx9math.h> // For D3DXVECTOR3

namespace core {
    namespace Threads {

        class cEntityList {
        private:
            std::mutex entityMutex;
            CPedInterFace* pedInterface;
            CPedList* pedList;

            std::atomic<bool> running;

            // Caching for expensive lookups
            struct PedCache {
                core::sdk::game::EntityStruct entity;
                std::chrono::steady_clock::time_point lastUpdate;
                std::string cachedName;
                std::string cachedWeapon;
                int lastHealth;
                int lastArmor;
                int lastMaxHealth;
                int lastPedType;
                int lastId;
                int lastIndex;
                bool lastIsFriend;
                bool lastIsVisible;
                float lastDistance;
                D3DXVECTOR3 lastPos;
                PedCache()
                    : lastHealth(-1), lastArmor(-1), lastMaxHealth(-1), lastPedType(-1),
                      lastId(-1), lastIndex(-1), lastIsFriend(false), lastIsVisible(false),
                      lastDistance(0.0f), lastPos{0.0f, 0.0f, 0.0f} {}
            };

            std::unordered_map<CPed*, PedCache> pedCacheMap;
            std::unordered_set<CPed*> lastSeenPeds;

            static const int kNameCacheMs = 5000;
            static const int kWeaponCacheMs = 500;

        public:
            std::vector<core::sdk::game::EntityStruct> LastEntitySnapshot;

            cEntityList()
                : pedInterface(nullptr), pedList(nullptr), running(true) {}

            void Stop() { running = false; }

            void Update() {
                while (running) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));

                    try {
                        if (!FetchPedInterface())
                            continue;

                        int maxPeds = 0;
                        if (pedInterface)
                            maxPeds = pedInterface->MaxPed();
                        if (maxPeds <= 0 || maxPeds > 2048)
                            continue;

                        UpdateEntities(maxPeds);
                    }
                    catch (const std::exception& e) {
                        std::cerr << "[EntityList] Crash avoided: " << e.what() << std::endl;
                    }
                    catch (...) {
                        std::cerr << "[EntityList] Unknown exception caught." << std::endl;
                    }
                }
            }

        private:
            bool FetchPedInterface() {
                auto* replay = core::sdk::Pointers::pReplayInterFace;
                if (!replay) return false;

                pedInterface = replay->InterfacePed();
                if (!pedInterface) return false;

                pedList = pedInterface->PedList();
                return pedList != nullptr;
            }

            void UpdateEntities(int maxPeds) {
                std::lock_guard<std::mutex> lock(entityMutex);

                lastSeenPeds.clear();
                std::vector<core::sdk::game::EntityStruct> entitySnapshot;
                entitySnapshot.reserve(static_cast<size_t>(maxPeds));

                auto now = std::chrono::steady_clock::now();

                for (int i = 0; i < maxPeds; ++i) {
                    CPed* ped = nullptr;
                    if (pedList)
                        ped = pedList->Ped(i);
                    if (!ped) continue;

                    lastSeenPeds.insert(ped);

                    PedCache& cache = pedCacheMap[ped];

                    // Only update static fields if ped changed or not set
                    if (cache.lastId == -1 || cache.entity.Ped != ped) {
                        cache.entity.Ped = ped;
                        cache.lastId = ped->GetID();
                        cache.entity.Id = cache.lastId;
                        cache.lastIndex = i;
                        cache.entity.Index = i;
                        cache.lastMaxHealth = ped->GetMaxHealth();
                        cache.entity.MaxHealth = cache.lastMaxHealth;
                        cache.lastPedType = ped->GetPedType();
                        cache.entity.PedType = cache.lastPedType;
                    }

                    // Dynamic fields
                    D3DXVECTOR3 pos;
                    if (ped)
                        pos = ped->GetPos();
                    else
                        pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

                    int health = ped ? ped->GetHealth() : 0;
                    int armor = ped ? ped->GetArmor() : 0;
                    bool isFriend = false;
                    if (core::sdk::game::FriendMap.find(ped) != core::sdk::game::FriendMap.end())
                        isFriend = core::sdk::game::FriendMap[ped];
                    bool isVisible = ped ? ped->IsVisible() : false;

                    cache.entity.Pos = pos;
                    cache.entity.Health = health;
                    cache.entity.Armor = armor;
                    cache.entity.IsFriend = isFriend;
                    cache.entity.IsVisible = isVisible;

                    auto* localPlayer = core::sdk::Pointers::pLocalPlayer;
                    float distance = 0.0f;
                    if (localPlayer && ped)
                        distance = ped->GetDistance(localPlayer->GetPos(), pos);
                    cache.entity.Distance = distance;

                    // Weapon name caching
                    if (cache.cachedWeapon.empty() ||
                        std::chrono::duration_cast<std::chrono::milliseconds>(now - cache.lastUpdate).count() > kWeaponCacheMs ||
                        health != cache.lastHealth)
                    {
                        cache.cachedWeapon = GetWeaponNameInternal(ped);
                        cache.lastUpdate = now;
                    }
                    cache.entity.WeaponName = cache.cachedWeapon;
                    cache.lastHealth = health;

                    // Ped name caching
                    if (cache.cachedName.empty() ||
                        std::chrono::duration_cast<std::chrono::milliseconds>(now - cache.lastUpdate).count() > kNameCacheMs)
                    {
                        cache.cachedName = GetPedNameSafeInternal(ped);
                        cache.lastUpdate = now;
                    }
                    cache.entity.NetworkInfo.UserName = cache.cachedName;

                    entitySnapshot.push_back(cache.entity);
                }

                // Remove stale cache entries
                for (auto it = pedCacheMap.begin(); it != pedCacheMap.end();) {
                    if (lastSeenPeds.find(it->first) == lastSeenPeds.end())
                        it = pedCacheMap.erase(it);
                    else
                        ++it;
                }

                // Update global entity list (thread-safe snapshot)
                LastEntitySnapshot = entitySnapshot;
                core::sdk::game::EntityList = LastEntitySnapshot;
            }

            static std::string GetWeaponNameInternal(CPed* ped) {
                try {
                    if (ped) {
                        auto* wm = ped->GetWeaponManager();
                        if (wm) {
                            auto* wi = wm->GetWeaponInfo();
                            if (wi)
                                return wi->GetName();
                        }
                    }
                } catch (...) {}
                return "None";
            }

            static std::string GetPedNameSafeInternal(CPed* ped) {
                try {
                    if (ped) {
                        std::string name = ped->GetPedName(ped);
                        if (name.empty())
                            return xorstr("NPC");
                        return name;
                    }
                }
                catch (...) {
                    return xorstr("NPC");
                }
                return xorstr("NPC");
            }
        };

        inline cEntityList g_EntityList;

    }
} 