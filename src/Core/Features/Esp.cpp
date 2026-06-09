#include "Esp.hpp"

#include <algorithm>
#include <optional>

std::optional<core::sdk::game::cSkeleton_t> LoadSkeleton(CPed* Ped)
{
	core::sdk::game::cSkeleton_t skeleton{};

	uintptr_t FragInstNMGta = driver.Read<uintptr_t>((uintptr_t)Ped + offsets.m_FragInst);
	if (!FragInstNMGta)
		return std::nullopt;

	uintptr_t v9 = driver.Read<uintptr_t>(FragInstNMGta + 0x68);
	if (!v9)
		return std::nullopt;

	skeleton.player_skeleton = driver.Read<uintptr_t>(v9 + 0x178);
	if (!skeleton.player_skeleton)
		return std::nullopt;

	skeleton.crSkeletonData.Ptr = driver.Read<uintptr_t>(skeleton.player_skeleton);
	if (!skeleton.crSkeletonData.Ptr)
		return std::nullopt;

	skeleton.crSkeletonData.m_Used = driver.Read<unsigned int>(skeleton.crSkeletonData.Ptr + 0x1A);
	skeleton.crSkeletonData.m_NumBones = driver.Read<unsigned int>(skeleton.crSkeletonData.Ptr + 0x5E);
	skeleton.crSkeletonData.m_bone_indexTable_Slots = driver.Read<unsigned short>(skeleton.crSkeletonData.Ptr + 0x18);

	if (!skeleton.crSkeletonData.m_bone_indexTable_Slots)
		return std::nullopt;

	skeleton.crSkeletonData.m_bone_indexTable = driver.Read<uintptr_t>(skeleton.crSkeletonData.Ptr + 0x10);
	skeleton.Arg1 = driver.Read<D3DXMATRIX>(driver.Read<uintptr_t>(skeleton.player_skeleton + 0x8));
	skeleton.Arg2 = driver.Read<uintptr_t>(skeleton.player_skeleton + 0x18);

	return skeleton;
}


void core::Features::cEsp::Draw()
{
	auto Cfg = g_Config.ESP;
	if (!Cfg->Enabled) return;

	static std::unordered_map<CPed*, core::sdk::game::EspAnim> vEspAnimations;
	static std::unordered_map<CPed*, core::sdk::game::cSkeleton_t> SkeletonCache;

	ImFont* defaultFont = ImGui::GetFont();
	float font_size = Cfg->NameSize;

	for (auto& Entity : core::sdk::game::EntityList)
	{
		CPed* Ped = Entity.Ped;
		auto& CurrentESPAnim = vEspAnimations[Ped];
		CurrentESPAnim.CanFadeOut = false;

		float Distance = Entity.Distance;
		float Health = Entity.Health;
		int PedType = Entity.PedType;
		bool IsFriend = Entity.IsFriend;
		bool IsLocalPlayer = Ped == core::sdk::Pointers::pLocalPlayer;

		if ((!Cfg->ShowLocalPlayer && IsLocalPlayer) ||
			(Distance > Cfg->MaxDistance) ||
			(Cfg->IgnoreNPCs && PedType != 2 && !IsLocalPlayer))
			continue;

		if (Cfg->IgnoreDead && !IsLocalPlayer) {
			if ((PedType == 2 && (Health > 400.f || Health <= 101.f)) ||
				(PedType != 2 && (Health > 200.f || Health <= 1.f)))
				continue;
		}

		auto maybeSkeleton = LoadSkeleton(Ped);
		if (!maybeSkeleton.has_value()) continue;
		auto& p_skeleton = SkeletonCache[Ped] = maybeSkeleton.value();

		auto get_bone_world_pos = [&](const std::string& bone) -> D3DXVECTOR3 {
			if (bone == "pelvis")        return core::sdk::game::get_bone_pos_r(Ped, SKEL_pelvis, p_skeleton);
			if (bone == "neck")          return core::sdk::game::get_bone_pos_r(Ped, SKEL_neck_1, p_skeleton);
			if (bone == "head")          return core::sdk::game::get_bone_pos_r(Ped, SKEL_Head, p_skeleton);
			if (bone == "leftupperarm")  return core::sdk::game::get_bone_pos_r(Ped, SKEL_L_UpperArm, p_skeleton);
			if (bone == "rightupperarm") return core::sdk::game::get_bone_pos_r(Ped, SKEL_R_UpperArm, p_skeleton);
			if (bone == "rightforearm")  return core::sdk::game::get_bone_pos_r(Ped, SKEL_R_Forearm, p_skeleton);
			if (bone == "leftforearm")   return core::sdk::game::get_bone_pos_r(Ped, SKEL_L_Forearm, p_skeleton);
			if (bone == "right_hand")    return core::sdk::game::get_bone_pos_r(Ped, SKEL_R_Hand, p_skeleton);
			if (bone == "left_hand")     return core::sdk::game::get_bone_pos_r(Ped, SKEL_L_Hand, p_skeleton);
			if (bone == "left_thigh")    return core::sdk::game::get_bone_pos_r(Ped, SKEL_L_Thigh, p_skeleton);
			if (bone == "left_calf")     return core::sdk::game::get_bone_pos_r(Ped, SKEL_L_Calf, p_skeleton);
			if (bone == "right_thigh")   return core::sdk::game::get_bone_pos_r(Ped, SKEL_R_Thigh, p_skeleton);
			if (bone == "right_calf")    return core::sdk::game::get_bone_pos_r(Ped, SKEL_R_Calf, p_skeleton);
			if (bone == "leftfoot")      return core::sdk::game::get_bone_pos_r(Ped, SKEL_L_Foot, p_skeleton);
			if (bone == "rightfoot")     return core::sdk::game::get_bone_pos_r(Ped, SKEL_R_Foot, p_skeleton);
			if (bone == "spine1")        return core::sdk::game::get_bone_pos_r(Ped, SKEL_Spine1, p_skeleton);
			if (bone == "spine2")        return core::sdk::game::get_bone_pos_r(Ped, SKEL_Spine2, p_skeleton);
			return D3DXVECTOR3(0.f, 0.f, 0.f);
		};

		auto get_bone_screen_pos = [&](const std::string& bone) -> D3DXVECTOR2 {
			return core::sdk::game::w2s(get_bone_world_pos(bone));
		};

		static const std::vector<std::string> bones = {
			"leftfoot", "rightfoot", "pelvis", "neck", "head", "leftupperarm",
			"rightupperarm", "rightforearm", "leftforearm", "right_hand", "left_hand",
			"left_thigh", "right_thigh", "right_calf", "spine1", "spine2"
		};

		std::vector<D3DXVECTOR2> bones_screen_pos;
		bones_screen_pos.reserve(bones.size());
		for (const auto& bone : bones)
			bones_screen_pos.push_back(get_bone_screen_pos(bone));

		if (!std::all_of(bones_screen_pos.begin(), bones_screen_pos.end(),
			[](const D3DXVECTOR2& pos) { return core::sdk::game::IsOnScreen(pos); }))
			continue;

		auto [min_x_it, max_x_it] = std::minmax_element(bones_screen_pos.begin(), bones_screen_pos.end(),
			[](const D3DXVECTOR2& a, const D3DXVECTOR2& b) { return a.x < b.x; });
		auto [min_y_it, max_y_it] = std::minmax_element(bones_screen_pos.begin(), bones_screen_pos.end(),
			[](const D3DXVECTOR2& a, const D3DXVECTOR2& b) { return a.y < b.y; });

		float min_x = min_x_it->x, max_x = max_x_it->x;
		float min_y = min_y_it->y, max_y = max_y_it->y;

		auto get = [&](const char* b) { return get_bone_screen_pos(b); };
		auto left_foot = get("leftfoot"), right_foot = get("rightfoot"), pelvis = get("pelvis"),
			neck = get("neck"), head = get("head"), left_up_arm = get("leftupperarm"),
			right_up_arm = get("rightupperarm"), right_fore_arm = get("rightforearm"),
			left_fore_arm = get("leftforearm"), right_hand = get("right_hand"),
			left_hand = get("left_hand"), left_thigh = get("left_thigh"),
			right_thigh = get("right_thigh"), right_calf = get("right_calf"),
			left_calf = get("left_calf"), spine1 = get("spine1"), spine2 = get("spine2");

		ImVec2 box_min(min_x, min_y), box_max(max_x, max_y);
		ImVec2 box_center((min_x + max_x) * 0.5f, (min_y + max_y) * 0.5f);
		ImVec2 box_center_top(box_center.x, min_y), box_center_bottom(box_center.x, max_y);

		float first_text_box_top = min_y - 18;
		float first_text_box_bottom = max_y + 4, second_text_box_bottom = max_y + 16, third_text_box_bottom = max_y + 28;
		int offset = (Cfg->HealthBarState == 2 && Cfg->ArmorBarState == 2) ? 16 :
					 (Cfg->HealthBarState == 2) ? 8 :
					 (Cfg->ArmorBarState == 2) ? 16 : 0;
		first_text_box_bottom += offset;
		second_text_box_bottom += offset;
		third_text_box_bottom += offset;
		if (Cfg->HealthBarState == 0 || Cfg->ArmorBarState == 0) first_text_box_top -= 4;

		if (IsFriend) continue;

		float padding = 4.0f;

		if (Cfg->UserNames) {
			const std::string& player_name = Entity.NetworkInfo.UserName;
			ImVec2 player_name_size = defaultFont->CalcTextSizeA(font_size, FLT_MAX, 0.0f, player_name.c_str());
			ImVec2 player_name_pos(box_center_top.x - player_name_size.x * 0.5f, box_center_top.y - player_name_size.y - padding);
			ImGui::PushFont(defaultFont);
			draw.outlined_text(player_name_pos, player_name.c_str(), ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 200), false, font_size, false);
			ImGui::PopFont();
		}

		if (Cfg->Box) {
			draw.outlined_rect(box_min, box_max, Cfg->BoxCol, Entity.IsVisible ? ImColor(0, 0, 0, 200) : ImColor(255, 113, 123, 200), 0.05f, ImColor(0, 0, 0, 200), false, 0);
		}

		if (Cfg->p_skeleton) {
			const float thickness = 0.5f;
			const ImColor color = Cfg->SkeletonCol;
			const bool outlined = true;
			const ImColor outline_col(0, 0, 0, 255);

			draw.line(ImVec2(left_up_arm.x, left_up_arm.y), ImVec2(neck.x, neck.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(right_up_arm.x, right_up_arm.y), ImVec2(neck.x, neck.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(right_up_arm.x, right_up_arm.y), ImVec2(right_fore_arm.x, right_fore_arm.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(left_up_arm.x, left_up_arm.y), ImVec2(left_fore_arm.x, left_fore_arm.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(right_fore_arm.x, right_fore_arm.y), ImVec2(right_hand.x, right_hand.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(left_fore_arm.x, left_fore_arm.y), ImVec2(left_hand.x, left_hand.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(neck.x, neck.y), ImVec2(pelvis.x, pelvis.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(pelvis.x, pelvis.y), ImVec2(left_thigh.x, left_thigh.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(pelvis.x, pelvis.y), ImVec2(right_thigh.x, right_thigh.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(left_thigh.x, left_thigh.y), ImVec2(left_calf.x, left_calf.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(right_thigh.x, right_thigh.y), ImVec2(right_calf.x, right_calf.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(left_calf.x, left_calf.y), ImVec2(left_foot.x, left_foot.y), color, thickness, outlined, outline_col);
			draw.line(ImVec2(right_calf.x, right_calf.y), ImVec2(right_foot.x, right_foot.y), color, thickness, outlined, outline_col);
		}

		if (Cfg->HeadCircle) {
			const float thickness = 0.5f;
			const ImColor color = Cfg->SkeletonCol;
			const bool outlined = true;
			const ImColor outline_col(0, 0, 0, 255);

			float dist = std::sqrt(std::pow(head.x - neck.x, 2) + std::pow(head.y - neck.y, 2));
			float radius = dist * 1.5f;

			draw.circle(ImVec2(head.x, head.y), radius, color, thickness, outlined, outline_col);
		}

		float weapon_name_height = 0.0f;
		if (Cfg->WeaponName) {
			std::string weapon_name = Entity.WeaponName;
			if (weapon_name == xorstr("UNARMED")) weapon_name = xorstr("Empty Handed");
			auto format_name = [](const std::string& str) -> std::string {
				if (str.empty()) return str;
				std::string result = str;
				std::transform(result.begin(), result.end(), result.begin(), ::tolower);
				result[0] = std::toupper(result[0]);
				return result;
			};
			if (weapon_name.empty()) weapon_name = xorstr("NONE");
			std::string formatted_name = format_name(weapon_name);
			ImVec2 weapon_text_size = defaultFont->CalcTextSizeA(font_size, FLT_MAX, 0.0f, formatted_name.c_str());
			weapon_name_height = weapon_text_size.y;
			ImVec2 weapon_text_pos(box_center_bottom.x - weapon_text_size.x * 0.5f, box_center_bottom.y + padding);
			ImGui::PushFont(defaultFont);
			draw.outlined_text(weapon_text_pos, formatted_name.c_str(), ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 200), false, font_size, false);
			ImGui::PopFont();
		}

		if (Cfg->DistanceFromMe) {
			std::string dist_str = std::to_string((int)Distance) + xorstr("m");
			ImVec2 dist_text_size = defaultFont->CalcTextSizeA(font_size, FLT_MAX, 0.0f, dist_str.c_str());
			float distance_y_pos = box_center_bottom.y + padding;
			if (Cfg->WeaponName) distance_y_pos += weapon_name_height + padding;
			ImVec2 dist_text_pos(box_center_bottom.x - dist_text_size.x * 0.5f, distance_y_pos);
			ImGui::PushFont(defaultFont);
			draw.outlined_text(dist_text_pos, dist_str.c_str(), ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 200), false, font_size, false);
			ImGui::PopFont();
		}

		if (Cfg->HealthBar) {
			const float bar_width = 2.0f, bar_spacing = 4.0f, bar_offset_left = 6.0f;
			const float min_health = 100.0f, max_health = 200.0f;
			float health = Entity.Health, armor = Entity.Armor;
			float box_height = box_max.y - box_min.y;
			float norm_health = ImClamp((health - min_health) / (max_health - min_health), 0.0f, 1.0f);
			float armor_max = max_health / 2.0f;
			float norm_armor = ImClamp(armor / armor_max, 0.0f, 1.0f);
			float health_bar_x = box_min.x - bar_offset_left;
			float armor_bar_x = health_bar_x;
			if (Cfg->ArmorBar && armor > 0) armor_bar_x -= (bar_width + bar_spacing);

			ImVec2 health_pos(health_bar_x, box_min.y), health_size(bar_width, box_height);
			draw.health_bar(health_pos, health_size, norm_health, ImColor(217, 242, 191), ImColor(217, 242, 191), ImColor(0, 0, 0));

			if (Cfg->ArmorBar && armor > 0) {
				ImVec2 armor_pos(armor_bar_x, box_min.y), armor_size(bar_width, box_height);
				draw.armor_bar(armor_pos, armor_size, norm_armor, ImColor(204, 229, 255), ImColor(204, 229, 255), ImColor(0, 0, 0));
			}
		}
	}
}


void core::Features::cEsp::DrawVehicle()
{
}