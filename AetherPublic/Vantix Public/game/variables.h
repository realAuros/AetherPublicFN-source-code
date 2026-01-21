#pragma once
#include <string>
#include <vector>
#include "imgui/imgui.h"
#include <memory>
struct cfg_data
{
	std::string label{};
	std::string desc{};
};


class c_variables
{
public:
	struct
	{
		ImVec2 size{ 620, 470 };
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		//ImVec2 padding{ 0, 0 };
		float border_size{ 1 };
		float scroll_bar_size{ 3 };
		float scroll_bar_rounding{ 100 };
		float window_rounding = 0.f;
	} window;

	struct
	{
		float dpi{ 1.3f };
		int stored_dpi{ 150 };
		bool dpi_changed{ true };

		bool global_pick_acitve{ false };

		float content_alpha{ 0.f };
		int active_section{ 0 };
		int section_count{ 0 };
		std::vector<std::string> section_labels{ "Aimbot", "Visuals", "Radar", "Exploits", "Settings" };
		std::vector<std::string>  section_icons{ "C", "D", "E", "F", "G" };

		float sub_content_alpha{ 0.f };
		std::vector<int> active_sub_section{ 0, 0, 0, 0, 0 };
		std::vector<int> sub_section_count{ 0, 0, 0, 0, 0 };
		std::vector<std::vector<std::string>> sub_section_labels
		{
			{  },
			{  },
			{  },
			{  },
			{  }
		};

		std::vector<cfg_data> configs;
		int current_config{ 0 };

		float anim_speed{ 1 };
	} gui;

	struct
	{
		ImFont* logo[1];
		ImFont* icons[7];
		ImFont* mono[4];
		ImFont* Term[6];
	} font;
};

inline std::unique_ptr<c_variables> var = std::make_unique<c_variables>();
