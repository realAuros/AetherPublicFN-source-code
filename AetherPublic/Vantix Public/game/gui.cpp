#include "gui.h"

static int keystatus = 0;
static int keystatus2 = 0;
static int keystatus3 = 0;
#define SCALE(...) scale_impl(__VA_ARGS__, var->gui.dpi)

inline ImVec2 scale_impl(const ImVec2& vec, float dpi) {
	return ImVec2(roundf(vec.x * dpi), roundf(vec.y * dpi));
}

inline ImVec2 scale_impl(float x, float y, float dpi) {
	return ImVec2(roundf(x * dpi), roundf(y * dpi));
}

inline float scale_impl(float var, float dpi) {
	return roundf(var * dpi);
}


void ChangeKey(void* blank)
{
    keystatus = 1;
    while (true)
    {
        for (int i = 0; i < 0x87; i++)
        {
            if (GetKeyState(i) & 0x8000)
            {
                settings.aim_b = i;

                keystatus = 0;
                return;
            }
        }
    }
}

static const char* keyNames[] =
{
	(""),
	("Left Mouse"),
	("Right Mouse"),
	("Cancel"),
	("Middle Mouse"),
	("Mouse 5"),
	("Mouse 4"),
	(""),
	("Backspace"),
	("Tab"),
	(""),
	(""),
	("Clear"),
	("Enter"),
	(""),
	(""),
	("Shift"),
	("Control"),
	("Alt"),
	("Pause"),
	("Caps"),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	("Escape"),
	(""),
	(""),
	(""),
	(""),
	("Space"),
	("Page Up"),
	("Page Down"),
	("End"),
	("Home"),
	("Left"),
	("Up"),
	("Right"),
	("Down"),
	(""),
	(""),
	(""),
	("Print"),
	("Insert"),
	("Delete"),
	(""),
	("0"),
	("1"),
	("2"),
	("3"),
	("4"),
	("5"),
	("6"),
	("7"),
	("8"),
	("9"),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	("A"),
	("B"),
	("C"),
	("D"),
	("E"),
	("F"),
	("G"),
	("H"),
	("I"),
	("J"),
	("K"),
	("L"),
	("M"),
	("N"),
	("O"),
	("P"),
	("Q"),
	("R"),
	("S"),
	("T"),
	("U"),
	("V"),
	("W"),
	("X"),
	("Y"),
	("Z"),
	(""),
	(""),
	(""),
	(""),
	(""),
	("Numpad 0"),
	("Numpad 1"),
	("Numpad 2"),
	("Numpad 3"),
	("Numpad 4"),
	("Numpad 5"),
	("Numpad 6"),
	("Numpad 7"),
	("Numpad 8"),
	("Numpad 9"),
	("Multiply"),
	("Add"),
	(""),
	("Subtract"),
	("Decimal"),
	("Divide"),
	("F1"),
	("F2"),
	("F3"),
	("F4"),
	("F5"),
	("F6"),
	("F7"),
	("F8"),
	("F9"),
	("F10"),
	("F11"),
	("F12")
};
static const char* keyNames2[] =
{
	(""),
	("Left Mouse"),
	("Right Mouse"),
	("Cancel"),
	("Middle Mouse"),
	("Mouse 5"),
	("Mouse 4"),
	(""),
	("Backspace"),
	("Tab"),
	(""),
	(""),
	("Clear"),
	("Enter"),
	(""),
	(""),
	("Shift"),
	("Control"),
	("Alt"),
	("Pause"),
	("Caps"),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	("Escape"),
	(""),
	(""),
	(""),
	(""),
	("Space"),
	("Page Up"),
	("Page Down"),
	("End"),
	("Home"),
	("Left"),
	("Up"),
	("Right"),
	("Down"),
	(""),
	(""),
	(""),
	("Print"),
	("Insert"),
	("Delete"),
	(""),
	("0"),
	("1"),
	("2"),
	("3"),
	("4"),
	("5"),
	("6"),
	("7"),
	("8"),
	("9"),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	(""),
	("A"),
	("B"),
	("C"),
	("D"),
	("E"),
	("F"),
	("G"),
	("H"),
	("I"),
	("J"),
	("K"),
	("L"),
	("M"),
	("N"),
	("O"),
	("P"),
	("Q"),
	("R"),
	("S"),
	("T"),
	("U"),
	("V"),
	("W"),
	("X"),
	("Y"),
	("Z"),
	(""),
	(""),
	(""),
	(""),
	(""),
	("Numpad 0"),
	("Numpad 1"),
	("Numpad 2"),
	("Numpad 3"),
	("Numpad 4"),
	("Numpad 5"),
	("Numpad 6"),
	("Numpad 7"),
	("Numpad 8"),
	("Numpad 9"),
	("Multiply"),
	("Add"),
	(""),
	("Subtract"),
	("Decimal"),
	("Divide"),
	("F1"),
	("F2"),
	("F3"),
	("F4"),
	("F5"),
	("F6"),
	("F7"),
	("F8"),
	("F9"),
	("F10"),
	("F11"),
	("F12")
};
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}
void HotkeyButton(int aimkey, void* changekey, int status)
{
	const char* preview_value = NULL;
	if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(keyNames))
		Items_ArrayGetter(keyNames, aimkey, &preview_value);

	std::string aimkeys;
	if (preview_value == NULL)
		aimkeys = ("None");
	else
		aimkeys = preview_value;

	if (status == 1)
	{
		aimkeys = ("Press the key");
	}
	if (ImGui::Button(aimkeys.c_str(), ImVec2(125, 22)))
	{
		if (status == 0)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
		}
	}
}

bool HotkeyButton2(std::string_view label, int* aimkey, void* changekey, int status)
{
	struct hotkey_state
	{
		float alpha{ 0 };
	};

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	const ImGuiID id = window->GetID(label.data());
	const ImVec2 pos = window->DC.CursorPos;
	ImVec2 button_size(125, 22);
	ImRect total(pos, pos + button_size);
	ImGui::ItemSize(total, style.FramePadding.y);
	if (!ImGui::ItemAdd(total, id, &total))
		return false;

	const char* preview_value = nullptr;
	if (*aimkey >= 0 && *aimkey < IM_ARRAYSIZE(keyNames2))
		Items_ArrayGetter(keyNames2, *aimkey, &preview_value);

	std::string aimkeys = (preview_value == nullptr) ? "None" : preview_value;

	char buf_display[64];
	sprintf_s(buf_display, " %s ", aimkeys.c_str());

	if (status == 1)
	{
		aimkeys = "Press the key";
	}


	ImU32 bgColor = ImColor(22, 22, 29, 0);

	ImGui::GetWindowDrawList()->AddRectFilled(total.Min, total.Max, bgColor, SCALE(5.f));

	// Tekst
	if (ImGui::Button(buf_display, button_size))
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
	}

	bool hovered = ItemHoverable(total, id);
	if (hovered && io.MouseClicked[0]) // Kliknięcie 
	{
		if (g.ActiveId != id)
		{
			ImGui::SetActiveID(id, window);
			ImGui::FocusWindow(window);
			*aimkey = -1;
		}
	}

	if (g.ActiveId == id)
	{
		for (int i = 0; i < 0x87; i++)
		{
			if (GetKeyState(i) & 0x8000)
			{
				*aimkey = i;
				ImGui::ClearActiveID();
				break;
			}
		}


		if (IsKeyPressed(ImGuiKey_Escape))
		{
			*aimkey = 0;
			ImGui::ClearActiveID();
		}
	}

	return true; // Zwróć
}
void render() {
	ImGui::SetNextWindowSize(var->window.size);

	ImGui::Begin(hash_string("Menu"), NULL, var->window.flags);
	{
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& p = ImGui::GetWindowPos();

        draw->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 620, p.y + 50), ImColor(9, 9, 9), 5, ImDrawCornerFlags_Top);
        draw->AddRectFilled(ImVec2(p.x, p.y + 50), ImVec2(p.x + 620, p.y + 51), ImColor(18, 18, 18), 5);
        draw->AddRectFilled(ImVec2(p.x, p.y + 51), ImVec2(p.x + 140, p.y + 470), ImColor(9, 9, 9), 5, ImDrawCornerFlags_BotLeft);
        draw->AddText(fonts::font, 24, ImVec2(p.x + 14, p.y + 12), ImColor(255, 255, 255), "Aether");
        draw->AddText(fonts::font, 24, ImVec2(p.x + 75, p.y + 12), ImColor(accentcolor), "Public (Beta)");

        ImGui::SetCursorPos(ImVec2(1, 55));
        ImGui::BeginGroup(); {

            ImGui::TextLine("GENERAL");
           /**/ if (ImGui::Tab("Aimbot", nullptr, 0 == var->gui.active_section))
                var->gui.active_section = 0;
            if (ImGui::Tab("Visuals", nullptr, 1 == var->gui.active_section))
                var->gui.active_section = 1;
            if (ImGui::Tab("Color", nullptr, 2 == var->gui.active_section))
                var->gui.active_section = 2;
            if (ImGui::Tab("Radar", nullptr, 3 == var->gui.active_section))
                var->gui.active_section = 3;
            if (ImGui::Tab("Exploits", nullptr, 4 == var->gui.active_section))
                var->gui.active_section = 4;

			if (ImGui::Tab("Settings", nullptr, 5 == var->gui.active_section))
				var->gui.active_section = 5;

        }ImGui::EndGroup();

        if (var->gui.active_section == 0) {
            ImGui::SetCursorPos(ImVec2(150, 25));
            e_elements::begin_child("Aimbot Settings", ImVec2(224, 435));
            ImGui::SetCursorPos(ImVec2(10, 40));
            ImGui::BeginGroup(); {
                ImGui::Checkbox(hash_string("Enable Aimbot"), &settings.aimbot);

                ImGui::Checkbox(hash_string("Humanize"), &settings.humanization);
                ImGui::SliderFloat(hash_string("Smoothing"), &settings.smoothsize, 1.f, 100.f);
                ImGui::Checkbox(hash_string("Show FOV Circle"), &settings.draw_fov);
                ImGui::SliderFloat(hash_string("FOV Size"), &settings.fovsize, 1.f, 1000.f);
				
				ImGui::SetCursorPosY(200);
				HotkeyButton2(hash_string("Aim Key").decrypt(), &settings.aim_b, ChangeKey, keystatus);
			}ImGui::EndGroup();
            e_elements::end_child();

			ImGui::SetCursorPos(ImVec2(384, 25));
			e_elements::begin_child("Visual Customization", ImVec2(224, 435));
			ImGui::SetCursorPos(ImVec2(10, 40));
			ImGui::BeginGroup(); {
				ImGui::MultiCombo("HitBone", settings.multi_bone, settings.bones, IM_ARRAYSIZE(settings.multi_bone));
			}ImGui::EndGroup();
			e_elements::end_child();
            
        }
        if (var->gui.active_section == 1) {
            ImGui::SetCursorPos(ImVec2(150, 25));
            e_elements::begin_child("Visual Settings", ImVec2(224, 435));
            ImGui::SetCursorPos(ImVec2(10, 40));
            ImGui::BeginGroup(); {
                ImGui::Checkbox(hash_string("Box"), &settings.box);
                ImGui::Checkbox(hash_string("Filled Box"), &settings.filled);
				ImGui::Checkbox(hash_string("Corner Box"), &settings.corner);
				ImGui::Checkbox(hash_string("Skeleton"), &settings.skel);
				ImGui::Checkbox(hash_string("Headline"), &settings.skel2);
				ImGui::Checkbox(hash_string("SnapLine"), &settings.snapline);
				ImGui::Checkbox(hash_string("Head ESP"), &settings.headesp);
				ImGui::Checkbox(hash_string("Rank ESP"), &settings.rankesp);
				ImGui::Checkbox(hash_string("Platform ESP"), &settings.platform);
				ImGui::Checkbox(hash_string("Weapon ESP"), &settings.weapon_hold);
				ImGui::Checkbox(hash_string("Distance ESP"), &settings.dist);


            }ImGui::EndGroup();
            e_elements::end_child();
			ImGui::SetCursorPos(ImVec2(384, 25));
			e_elements::begin_child("Visual Customization", ImVec2(224, 435));
			ImGui::SetCursorPos(ImVec2(10, 40));
			ImGui::BeginGroup(); {
				ImGui::Checkbox(hash_string("Box Outline"), &settings.box_outline);
				ImGui::Checkbox(hash_string("Skeleton Outline"), &settings.skel_outline);
				ImGui::SliderFloat(hash_string("Outline Value"), &settings.box_outline_value, 0.5f, 4.f, "%.1f");
				ImGui::Checkbox(hash_string("Smooth Skeleton"), &settings.chinesehat);
			}ImGui::EndGroup();
			e_elements::end_child();
        }
        if (var->gui.active_section == 2) {
            ImGui::SetCursorPos(ImVec2(150, 25));
            e_elements::begin_child("Color Settings", ImVec2(224, 435));
			ImGui::SetCursorPos(ImVec2(10, 40));
			ImGui::BeginGroup(); {
				ImGui::ColorEdit4("##Accent color", (float*)&accentcolor, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
				ImGui::SetCursorPos(ImVec2(10, 39));
				ImGui::PushFont(fonts::Regylar);
				ImGui::Text("Accent color");
				ImGui::PopFont();
				ImGui::ColorEdit4("##Skeleton color", (float*)&settings.Skelvis, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
				ImGui::SetCursorPos(ImVec2(10, 89));
				ImGui::PushFont(fonts::Regylar);
				ImGui::Text("Skeleton color");
				ImGui::PopFont();

			}ImGui::EndGroup();
            e_elements::end_child();
           
        }
        if (var->gui.active_section == 3) {
            ImGui::SetCursorPos(ImVec2(150, 25));
            e_elements::begin_child("Radar", ImVec2(224, 435));
			ImGui::BeginGroup(); {
				ImGui::SetCursorPos(ImVec2(10, 40));

			ImGui::Checkbox("Enable Radar", &settings.Radar);
			}ImGui::EndGroup();

            e_elements::end_child();
            ImGui::SetCursorPos(ImVec2(384, 25));
            e_elements::begin_child("Radar Settings", ImVec2(224, 435));
			ImGui::BeginGroup(); {
				ImGui::SetCursorPos(ImVec2(10, 40));

			ImGui::SliderFloat("X", &settings.radar_pos_x, 0.f, 1000.f, "%.1f");
			ImGui::SliderFloat("Y", &settings.radar_pos_y, 0.f, 1000.f, "%.1f");
			ImGui::SliderFloat("Radar Size", &settings.radar_size, 0.f, 1000.f, "%.1f");
			ImGui::SliderFloat("Radar Range", &settings.radar_range, 0.f, 1000.f, "%.1f");
			}ImGui::EndGroup();

            e_elements::end_child();
        }
        if (var->gui.active_section == 4) {
            ImGui::SetCursorPos(ImVec2(150, 25));
            e_elements::begin_child("Exploits", ImVec2(224, 435));
			ImGui::BeginGroup(); {
				ImGui::SetCursorPos(ImVec2(10, 40));
			    ImGui::Checkbox("Spinbot", &settings.chams);
			    ImGui::Checkbox("FOV Changer", &settings.FovChanger);
		    }ImGui::EndGroup();

            e_elements::end_child();
            ImGui::SetCursorPos(ImVec2(384, 25));
            e_elements::begin_child("Exploit Setting", ImVec2(224, 435));
			ImGui::BeginGroup(); {
				ImGui::SetCursorPos(ImVec2(10, 40));
			ImGui::SliderFloat("FOV Value", &settings.valuefov, 1.f, 190.f, "%.1f");
						}ImGui::EndGroup();

            e_elements::end_child();
        }
		if (var->gui.active_section == 5) {
			ImGui::SetCursorPos(ImVec2(150, 25));
			e_elements::begin_child("Settings", ImVec2(224, 435));
			ImGui::BeginGroup(); {
				ImGui::SetCursorPos(ImVec2(10, 40));
				ImGui::Checkbox("vSync", &settings.vsync);
				if (ImGui::Button("Exit", ImVec2(100, 40)))
					exit(0);
			}ImGui::EndGroup();

			e_elements::end_child();
			
		}
        
	}
	ImGui::End();
}