#define _CRT_SECURE_NO_WARNINGS


#include <list>
#include <string>
#include "../offsets.h"
#include "../sdk./functions.h"
#include "../aimbot/aimbot.h"
#include <iostream>
#include <vector>
#include <immintrin.h>
#include <d3d11.h>
#include "../utils.h"
#include "../xorstr.h"
#include "../imgui/imgui.h"
#include <d3d11.h>

D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11ShaderResourceView* Box2 = nullptr;
ID3D11ShaderResourceView* Headesp1 = nullptr;
ID3D11ShaderResourceView* Robert2 = nullptr;
ImFont* esp;
ImFont* esp2;
std::mutex base;
float TargetDistance = FLT_MAX;
uintptr_t TargetEntity = NULL;
uintptr_t DesyncTargetEntity = NULL;


int gun_type;

struct weapon_info_
{
	std::string weapon_name;
};
std::string LocalPlayerWeapon;
std::string RankedProgress;
weapon_info_ weapon_info;

__forceinline weapon_info_ get_weapon_name(uint64_t actor) {
	uintptr_t current_weapon = read<uintptr_t>(actor + offsets::CurrentWeapon);

	uint64_t weapon_data = read<uint64_t>(current_weapon + offsets::weapondata);
	if (!weapon_data) return { };

	uint64_t fname_text = read<uint64_t>(weapon_data + 0x40);
	if (!fname_text) return { };
	uint32_t name_length = read<uint32_t>(fname_text + 0x20);
	if (!name_length) return { };

	wchar_t* WeaponName = new wchar_t[uint64_t(name_length) + 1];

	mem::read_physical(read<uintptr_t>(fname_text + 0x18),

		WeaponName,
		name_length * sizeof(wchar_t));

	std::wstring wWeaponName(WeaponName);
	delete[] WeaponName;

	weapon_info_ weapon_info;
	weapon_info.weapon_name = std::string(wWeaponName.begin(), wWeaponName.end());
	return weapon_info;
}

auto get_platform(uintptr_t ps) {

	uintptr_t test_platform = read<uintptr_t>(ps + 0x430); //FortPlayerState.Platform

	wchar_t platform[64] = { 0 };

	read_array((test_platform),
		reinterpret_cast<uint8_t*>(platform),
		sizeof(platform));

	std::wstring platform_wstr(platform);

	std::string platform_str(platform_wstr.begin(), platform_wstr.end());

	return platform_str;
}

fvector PlayerFlyAddValue; //outside the loop
fvector PlayerFlySetLocation; //outside the loop

void Draw_Watermark(const std::string& text, const ImVec2& pos, float size, ImU32 color)
{
	constexpr float fStrokeVal1 = 1.0f;
	uint32_t EdgeColor = 0xFF000000;
	float Edge_a = (EdgeColor >> 24) & 0xff;
	float Edge_r = (EdgeColor >> 16) & 0xff;
	float Edge_g = (EdgeColor >> 8) & 0xff;
	float Edge_b = (EdgeColor) & 0xff;
	std::stringstream steam(text);
	std::string line;
	float y = 0.0f;
	int i = 0;
	while (std::getline(steam, line))
	{
		ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x) - fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x) + fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2(pos.x, pos.y + textSize.y * i), color, line.c_str());

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
}

void Draw_Text(const std::string& text, const ImVec2& pos, float size, ImU32 color)
{
	constexpr float fStrokeVal1 = 1.0f;
	uint32_t EdgeColor = 0xFF000000;
	float Edge_a = (EdgeColor >> 24) & 0xff;
	float Edge_r = (EdgeColor >> 16) & 0xff;
	float Edge_g = (EdgeColor >> 8) & 0xff;
	float Edge_b = (EdgeColor) & 0xff;
	std::stringstream steam(text);
	std::string line;
	float y = 0.0f;
	int i = 0;
	while (std::getline(steam, line))
	{
		ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - textSize.x / 2.0f) - fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - textSize.x / 2.0f) + fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
}


std::string getRank(int tier) {
	switch (tier) {
	case 0:  return "Unranked";
	case 1:  return "Bronze 2";
	case 2:  return "Bronze 3";
	case 3:  return "Silver 1";
	case 4:  return "Silver 2";
	case 5:  return "Silver 3";
	case 6:  return "Gold 1";
	case 7:  return "Gold 2";
	case 8:  return "Gold 3";
	case 9:  return "Platinum 1";
	case 10: return "Platinum 2";
	case 11: return "Platinum 3";
	case 12: return "Diamond 1";
	case 13: return "Diamond 2";
	case 14: return "Diamond 3";
	case 15: return "Elite";
	case 16: return "Champion";
	case 17: return "Unreal";
	default: return "Unranked";
	}
}

ImVec4 getRankColor(int tier) {

	switch (tier) {
	case 0:  return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	case 3:  return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	case 2:  return ImVec4(0.8f, 0.5f, 0.2f, 1.0f);
	case 1:  return ImVec4(0.8f, 0.5f, 0.2f, 1.0f);
	case 5:  return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	case 6:  return ImVec4(1.0f, 0.843f, 0.0f, 1.0f);
	case 4:  return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	case 8:  return ImVec4(1.0f, 0.843f, 0.0f, 1.0f);
	case 7:  return ImVec4(1.0f, 0.843f, 0.0f, 1.0f);
	case 9:  return ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
	case 10: return ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
	case 11: return ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
	case 14: return ImVec4(0.7f, 0.4f, 1.0f, 1.0f);
	case 12: return ImVec4(0.7f, 0.4f, 1.0f, 1.0f);
	case 15: return ImVec4(0.9f, 0.85f, 0.6f, 1.0f);
	case 13: return ImVec4(0.7f, 0.4f, 1.0f, 1.0f);
	case 16: return ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
	case 17: return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);
	default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void draw_player_rank(Vector3 location, int tier)
{

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2 + -1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), getRank(tier).c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2 + 1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), getRank(tier).c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2 + -1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), getRank(tier).c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2 + 1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), getRank(tier).c_str());

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2, location.y), ImGui::ColorConvertFloat4ToU32(getRankColor(tier)), getRank(tier).c_str());
}

int debug = 0;
void unhideconsolewindow() {

	HWND hConsole = GetConsoleWindow();
	ShowWindow(hConsole, SW_SHOW);  // zchowaj kurwo wioe,wm,gwe,mg

}
void niga() {
	for (;; )
	{


		temporary_entity_list.clear();
		std::lock_guard<std::mutex> lock(base);

		CachePointers.UWorld = read<__int64>(virtualaddy + offsets::UWorld);
		CachePointers.GameInstance = read<uintptr_t>(CachePointers.UWorld + offsets::OwningGameInstance);
		CachePointers.GameState = read<uintptr_t>(CachePointers.UWorld + offsets::GameState);
		CachePointers.LocalPlayer = read<uintptr_t>(read<uintptr_t>(CachePointers.GameInstance + offsets::LocalPlayers));
		CachePointers.PlayerController = read<uintptr_t>(CachePointers.LocalPlayer + offsets::PlayerController);
		CachePointers.AcknownledgedPawn = read<uintptr_t>(CachePointers.PlayerController + offsets::AcknowledgedPawn);
		CachePointers.Mesh = read<uintptr_t>(CachePointers.AcknownledgedPawn + offsets::Mesh);
		CachePointers.PlayerState = read<uintptr_t>(CachePointers.AcknownledgedPawn + offsets::PlayerState);
		CachePointers.RootComponent = read<uintptr_t>(CachePointers.AcknownledgedPawn + offsets::RootComponent);
		CachePointers.PlayerArray = read<uintptr_t>(CachePointers.GameState + offsets::PlayerArray);
		CachePointers.PlayerArraySize = read<int>(CachePointers.GameState + (offsets::PlayerArray + sizeof(uintptr_t)));
		CachePointers.LocalWeapon = read<uintptr_t>(CachePointers.AcknownledgedPawn + offsets::CurrentWeapon);
		CachePointers.TeamIndex = read<uintptr_t>(CachePointers.PlayerState + offsets::Team_Index);
		CachePointers.RelativeLocation = read<fvector>(CachePointers.RootComponent + offsets::RelativeLocation);



		//unhideconsolewindow();
		if (debug == 1) {
			std::cout << "========================================" << std::endl;
			std::cout << "Uworld: " << CachePointers.UWorld << std::endl;
			std::cout << "GameInstance: " << CachePointers.GameInstance << std::endl;
			std::cout << "GameState: " << CachePointers.GameState << std::endl;
			std::cout << "LocalPlayer: " << CachePointers.LocalPlayer << std::endl;
			std::cout << "PlayerController: " << CachePointers.PlayerController << std::endl;
			std::cout << "AcknownledgedPawn: " << CachePointers.AcknownledgedPawn << std::endl;
			std::cout << "Mesh: " << CachePointers.Mesh << std::endl;
			std::cout << "PlayerState: " << CachePointers.PlayerState << std::endl;
			std::cout << "RootComponent: " << CachePointers.RootComponent << std::endl;
			std::cout << "PlayerArray: " << CachePointers.PlayerArray << std::endl;
			std::cout << "PlayerArraySize: " << CachePointers.PlayerArraySize << std::endl;
			std::cout << "LocalWeapon: " << CachePointers.LocalWeapon << std::endl;
			std::cout << "TeamIndex: " << CachePointers.TeamIndex << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}
}

uint8_t IsKnocked(uint64_t Actor)
{
	// 0x912 is FortniteGame::FortPawn::bIsDBNO

	uint8_t bIsDBNO = 0U;
	bIsDBNO = read<uint8_t>(Actor + 0x841) & 0x20;

	return bIsDBNO;
}

auto InScreen(fvector2d screen_position) -> bool {

	if (screen_position.x > 0 && screen_position.x < globals.ScreenWidth && screen_position.y > 0 && screen_position.y < globals.ScreenHeight)
		return true;
	else
		return false;
}
#include <chrono>
#include <thread>
void RadarRange(float* x, float* y, float range) {
	if (fabs((*x)) > range || fabs((*y)) > range) {
		if ((*y) > (*x)) {
			if ((*y) > -(*x)) {
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else {
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else {
			if ((*y) > -(*x)) {
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else {
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}

void CalcRadarPoint(fvector vOrigin, int& screenx, int& screeny) {
	fvector vAngle = camera_postion.rotation;
	auto fYaw = vAngle.y * M_PI / 180.0f;
	float dx = vOrigin.x - camera_postion.location.x;
	float dy = vOrigin.y - camera_postion.location.y;

	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	x = -x;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = (float)settings.radar_range * 1000.f;
	RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImVec2(settings.radar_pos_x, settings.radar_pos_y);
	ImVec2 DrawSize = ImVec2(settings.radar_size, settings.radar_size);

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;


	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));


	if (screenx > x_max)
		screenx = x_max;
	if (screenx < rad_x)
		screenx = rad_x;
	if (screeny > y_max)
		screeny = y_max;
	if (screeny < rad_y)
		screeny = rad_y;
}

void addToFovRadar(fvector WorldLocation, ImVec2 drawPos, float FOVScale, ImColor color)
{


}

void AddPlayerToFovCircle(Vector3 WorldLocation, float fDistance, bool visible, ImColor color)
{
	// Arrow color is always red
	ImColor arrowColor(255, 0, 0); // Red color for the arrow

	fvector vAngle = camera_postion.rotation;
	float fYaw = vAngle.y * M_PI / 180.0f;
	float dx = WorldLocation.x - camera_postion.location.x;
	float dy = WorldLocation.y - camera_postion.location.y;
	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);

	auto Center = ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2);

	float x = -(dy * fminus_cos_yaw + dx * fsin_yaw);
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float fovRadius = settings.fovsize;
	float angle = atan2f(y, x);
	float triangleSize = 12.0f;
	float widthFactor = 8.0f;
	float outlineThickness = 3.0f;

	ImVec2 triangleCenter = ImVec2(Center.x + cosf(angle) * (fovRadius + triangleSize),
		Center.y + sinf(angle) * (fovRadius + triangleSize));

	ImVec2 point1 = ImVec2(triangleCenter.x + cosf(angle) * triangleSize,
		triangleCenter.y + sinf(angle) * triangleSize);
	ImVec2 point2 = ImVec2(triangleCenter.x + cosf(angle + widthFactor) * triangleSize,
		triangleCenter.y + sinf(angle + widthFactor) * triangleSize);
	ImVec2 point3 = ImVec2(triangleCenter.x + cosf(angle - widthFactor) * triangleSize,
		triangleCenter.y + sinf(angle - widthFactor) * triangleSize);


	ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(arrowColor);


	ImU32 outlineColor = ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 1.0));

	ImDrawList* drawList = ImGui::GetForegroundDrawList();


	drawList->AddTriangle(point1, point2, point3, outlineColor, outlineThickness);


	drawList->AddTriangleFilled(point1, point2, point3, fillColor);
}

void render_radar_main() {
	// pos & size
	ImVec2 radar_pos(settings.radar_pos_x, settings.radar_pos_y);
	float radar_radius = settings.radar_size / 2.0f;

	// background 




	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
	ImVec2 background_pos(radar_pos.x + radar_radius, radar_pos.y + radar_radius);
	draw_list->AddCircleFilled(background_pos, radar_radius, ImGui::GetColorU32(ImVec4(22.0f / 255.0f, 22.0f / 255.0f, 22.0f / 255.0f, 0.7f)));






	float circle_radius_step = (radar_radius - 10.0f) / 4.0f;


	for (int i = 1; i <= 4; ++i) {
		float current_radius = circle_radius_step * i;
		draw_list->AddCircle(background_pos, current_radius, ImGui::GetColorU32({ 1.f, 1.f, 1.f, 0.6f }), 100, 2.0f); //  axis circle
	}

	// player pos
	ImVec2 center(radar_pos.x + radar_radius, radar_pos.y + radar_radius);
	draw_list->AddCircleFilled(center, 3.f, ImGui::GetColorU32({ 0.f, 0.f, 1.f, 1.f }), 12); //  marker
}

void add_to_radar(fvector WorldLocation, float fDistance, bool visible) {
	int ScreenX, ScreenY = 0;
	CalcRadarPoint(WorldLocation, ScreenX, ScreenY);



	if (!visible) {
		// Invisible
		ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenX, ScreenY), 4.f, ImGui::GetColorU32({ 255.f / 255.f, 0.f / 255.f, 0.f / 255.f, 1.f }), 12);
		//ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenX, ScreenY), 6.f, ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), 12);
	}
	else {
		// Visible
		ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenX, ScreenY), 4.f, ImGui::GetColorU32({ 0.f / 255.f, 255.f / 255.f, 0.f / 255.f, 1.f }), 12);
	}




	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(ScreenX, ScreenY), 4.f, ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 12, 1.f);


	if (settings.dist) {

		float bottom_offset = 7.f;
		ImVec2 textPosition(ScreenX, ScreenY + bottom_offset);


		std::string distance_str = std::to_string(static_cast<int>(fDistance)) + "m";
		char buffer[128];
		sprintf_s(buffer, hash_string("%s"), distance_str.c_str());


		Draw_Text(buffer, textPosition, settings.size_font, ImColor(settings.distance[0], settings.distance[1], settings.distance[2], settings.distance[3]));
	}
}

void DrawBezierCurve(ImDrawList* drawList, ImVec2 p0, ImVec2 p1, ImVec2 p2, ImU32 color, float thickness)
{
	const int segments = 90; // More segments = smoother curve
	for (int i = 0; i < segments; ++i)
	{
		float t1 = (float)i / (float)segments;
		float t2 = (float)(i + 1) / (float)segments;

		ImVec2 point1 = ImVec2(
			(1 - t1) * (1 - t1) * p0.x + 2 * (1 - t1) * t1 * p1.x + t1 * t1 * p2.x,
			(1 - t1) * (1 - t1) * p0.y + 2 * (1 - t1) * t1 * p1.y + t1 * t1 * p2.y
		);

		ImVec2 point2 = ImVec2(
			(1 - t2) * (1 - t2) * p0.x + 2 * (1 - t2) * t2 * p1.x + t2 * t2 * p2.x,
			(1 - t2) * (1 - t2) * p0.y + 2 * (1 - t2) * t2 * p1.y + t2 * t2 * p2.y
		);

		drawList->AddLine(point1, point2, color, thickness);
	}
}
std::vector<ImVec2> GetCatmullRomSpline(const std::vector<ImVec2>& controlPoints, int segments)
{
	std::vector<ImVec2> spline;
	if (controlPoints.size() < 2)
		return spline;

	// Duplicate the endpoints to better handle endpoints.
	std::vector<ImVec2> pts;
	pts.push_back(controlPoints.front());
	pts.insert(pts.end(), controlPoints.begin(), controlPoints.end());
	pts.push_back(controlPoints.back());

	// For each segment between pts[i] and pts[i+1], interpolate using pts[i-1], pts[i], pts[i+1], pts[i+2]
	for (size_t i = 1; i < pts.size() - 2; i++)
	{
		for (int j = 0; j < segments; j++)
		{
			float t = j / (float)segments;
			float t2 = t * t;
			float t3 = t2 * t;
			ImVec2 p0 = pts[i - 1];
			ImVec2 p1 = pts[i];
			ImVec2 p2 = pts[i + 1];
			ImVec2 p3 = pts[i + 2];

			ImVec2 point;
			point.x = 0.5f * ((2 * p1.x) + (-p0.x + p2.x) * t + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3);
			point.y = 0.5f * ((2 * p1.y) + (-p0.y + p2.y) * t + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3);
			spline.push_back(point);
		}
	}
	// Ensure the last control point is added.
	spline.push_back(pts[pts.size() - 2]);
	return spline;
}
enum EBoneIndex : int
{
	Head = 110,
	Neck = 67,
	Chest = 66,
	Pelvis = 2,
	LShoulder = 9,
	LElbow = 10,
	LHand = 11,
	RShoulder = 38,
	RElbow = 39,
	RHand = 40,
	LHip = 71,
	LKnee = 72,
	LFoot = 75,
	RHip = 78,
	RKnee = 79,
	RFoot = 82,
	BASE = 0
}; //

void DrawSkeleton(uintptr_t Mesh, ImColor color, bool curved, bool spaghetti)
{
	auto drawList = ImGui::GetForegroundDrawList();
	ImColor outlineColor = { 0,0,0,255 };
	float outlineThickness = 2;
	float thickness = 1;

	if (curved && !spaghetti)
	{
		// Compute shoulder center
		fvector leftShoulder3D = game_helper.GetBoneLocation(Mesh, EBoneIndex::LShoulder);
		fvector rightShoulder3D = game_helper.GetBoneLocation(Mesh, EBoneIndex::RShoulder);

		fvector2d leftShoulderScreen = game_helper.ProjectWorldToScreen(leftShoulder3D);
		fvector2d rightShoulderScreen = game_helper.ProjectWorldToScreen(rightShoulder3D);
		ImVec2 centerShoulders((leftShoulderScreen.x + rightShoulderScreen.x) / 2.0f,
			(leftShoulderScreen.y + rightShoulderScreen.y) / 2.0f);

		// Build continuous bone chains with arms using the computed center:
		// Left arm: center -> LShoulder -> LElbow -> LHand
		std::vector<ImVec2> leftArmChain;
		leftArmChain.push_back(centerShoulders);
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::LShoulder);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			leftArmChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::LElbow);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			leftArmChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::LHand);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			leftArmChain.push_back(ImVec2(scr.x, scr.y));
		}

		// Right arm: center -> RShoulder -> RElbow -> RHand
		std::vector<ImVec2> rightArmChain;
		rightArmChain.push_back(centerShoulders);
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::RShoulder);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			rightArmChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::RElbow);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			rightArmChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::RHand);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			rightArmChain.push_back(ImVec2(scr.x, scr.y));
		}

		// Left leg: Pelvis -> LHip -> LKnee -> LFoot
		std::vector<ImVec2> leftLegChain;
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::Pelvis);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			leftLegChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::LHip);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			leftLegChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::LKnee);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			leftLegChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::LFoot);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			leftLegChain.push_back(ImVec2(scr.x, scr.y));
		}

		// Right leg: Pelvis -> RHip -> RKnee -> RFoot
		std::vector<ImVec2> rightLegChain;
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::Pelvis);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			rightLegChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::RHip);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			rightLegChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::RKnee);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			rightLegChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::RFoot);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			rightLegChain.push_back(ImVec2(scr.x, scr.y));
		}

		// Spine: Neck -> Pelvis
		std::vector<ImVec2> spineChain;
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::Neck);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			spineChain.push_back(ImVec2(scr.x, scr.y));
		}
		{
			fvector pos = game_helper.GetBoneLocation(Mesh, EBoneIndex::Pelvis);
			fvector2d scr = game_helper.ProjectWorldToScreen(pos);
			spineChain.push_back(ImVec2(scr.x, scr.y));
		}

		// Collect all chains to draw.
		std::vector<std::vector<ImVec2>> chainsToDraw = { leftArmChain, rightArmChain, leftLegChain, rightLegChain, spineChain };

		// For each chain, generate and draw a Catmull-Rom spline.
		for (const auto& controlPoints : chainsToDraw)
		{
			if (controlPoints.size() < 2)
				continue;
			std::vector<ImVec2> splinePoints = GetCatmullRomSpline(controlPoints, 10);
			if (splinePoints.size() < 2)
				continue;
			if (settings.SkelThickness)
				drawList->AddPolyline(splinePoints.data(), splinePoints.size(), outlineColor, false, outlineThickness + settings.SkelThickness);

			drawList->AddPolyline(splinePoints.data(), splinePoints.size(), color, false, settings.SkelThickness);
		}
	}
	
	
}


enum bone : int
{
	HumanBase = 0,
	HumanPelvis = 2,
	HumanLThigh1 = 71,
	HumanLThigh2 = 77,
	HumanLThigh3 = 72,
	HumanLCalf = 74,
	HumanLFoot2 = 73,
	HumanLFoot = 86,
	HumanLToe = 76,
	HumanRThigh1 = 78,
	HumanRThigh2 = 84,
	HumanRThigh3 = 79,
	HumanRCalf = 81,
	HumanRFoot2 = 82,
	HumanRFoot = 87,
	HumanRToe = 83,
	HumanSpine1 = 7,
	HumanSpine2 = 5,
	HumanSpine3 = 2,
	HumanLCollarbone = 9,
	HumanLUpperarm = 35,
	HumanLForearm1 = 36,
	HumanLForearm23 = 10,
	HumanLForearm2 = 34,
	HumanLForearm3 = 33,
	HumanLPalm = 32,
	HumanLHand = 11,
	HumanRCollarbone = 98,
	HumanRUpperarm = 64,
	HumanRForearm1 = 65,
	HumanRForearm23 = 39,
	HumanRForearm2 = 63,
	HumanRForearm3 = 62,
	HumanRHand = 40,
	HumanRPalm = 58,
	HumanNeck = 67,
	HumanHead = 110,
	HumanLowerHead = 106,

	HumanChest = 7
};

inline void actorloop()
{
	TargetDistance = FLT_MAX;
	TargetEntity = NULL;
	DesyncTargetEntity = NULL;

	//ImGui::PushFont(esp);
//
	char water[128];
	sprintf_s(water, hash_string("ARENA Private"));
	//Draw_Text(water, ImVec2(40, 3), 20.f, ImColor(255, 255, 255, 255));

	ImGuiIO& io = ImGui::GetIO();
	static float timeElapsed = 0.0f;
	static int frameCount = 0;
	static float fps = 0.0f;
	const float updateInterval = 0.005f;
	static std::string currentTime;
	static double lastUpdate = 0.0;

	double now = ImGui::GetTime();

	static int lastMinute = -1; // track the last rendered minute

	// Get the current system time
	time_t t = time(nullptr);
	tm* localTime = localtime(&t);

	frameCount++;
	timeElapsed += io.DeltaTime;

	if (timeElapsed >= updateInterval)
	{
		fps = frameCount / timeElapsed;
		frameCount = 0;
		timeElapsed = 0.0f;
	}

	char fpsText[30];

	std::snprintf(fpsText, sizeof(fpsText), "FPS: %.2f", fps); // float.
	//Draw_Text(fpsText, ImVec2(60, 22), 20.f, ImColor(255, 255, 255, 255));
	// Update every 60 seconds
	if (localTime->tm_min != lastMinute)
	{
		char buffer[32];
		strftime(buffer, sizeof(buffer), "%I:%M %p", localTime); // e.g., 03:45 PM
		currentTime = buffer;

		lastMinute = localTime->tm_min;
	}
	//Draw_Text(currentTime.c_str(), ImVec2(60, 60), 18.f, ImColor(255, 255, 255, 255));
	sprintf_s(water, hash_string("Aether Public"));
	//Draw_Text(water, ImVec2(60, 1030), 18.f, ImColor(255, 255, 255, 255));
	ImDrawList* draw = ImGui::GetBackgroundDrawList();


	ImVec2 topleft_(10, 10);
	ImVec2 bottomright_(200, 75);

	draw->AddRectFilled(topleft_, bottomright_, ImColor(11, 9, 23, 220), 8.f);
	//draw->AddText(ImVec2(13, 5), ImColor(230, 230, 230, 255), "Arena Ultimate");
	//draw->AddText(ImVec2(30, 5), ImColor(230, 230, 230, 255), currentTime.c_str());
	//draw->AddText(ImVec2(13, 5), ImColor(230, 230, 230, 255), "FPS:");
	draw->AddText(ImVec2(13, 13), ImColor(230, 230, 230, 255), fpsText);
	draw->AddText(ImVec2(13, 30), ImColor(230, 230, 230, 255), "Aether Public");

	//	std::string expireDate = TrashAuth::get_expiredate; //string v
		//std::string expireText = "Expire: " + expireDate;
	//	Draw_Text(expireText, ImVec2(95, 1050), 18.f, ImColor(255, 255, 255, 255));


	//ImGui::PopFont();


	//ImGui::PushFont(esp2);

	for (int i = 0; i < CachePointers.PlayerArraySize; ++i)
	{
		uintptr_t PlayerState = read<uintptr_t>(CachePointers.PlayerArray + (i * sizeof(uintptr_t)));
		uintptr_t CurrentActor = read<uintptr_t>(PlayerState + offsets::PawnPrivate);


		uint64_t CurrentActorMesh = read<uint64_t>(CurrentActor + offsets::Mesh);
		uint64_t PlayerController = read<uint64_t>(CurrentActor + offsets::PlayerController);
		DWORD64 otherPlayerState = read<uint64_t>(CurrentActor + offsets::PlayerState);
		DWORD64 otherPlayerPawn = read<uint64_t>(CurrentActor + offsets::AcknowledgedPawn);

		if (CurrentActor == CachePointers.AcknownledgedPawn) continue;

		if (CachePointers.AcknownledgedPawn) {
			if (settings.wpn_cfg) {
				weapon_info_ local_held_weapon = get_weapon_name(CachePointers.AcknownledgedPawn);
				LocalPlayerWeapon = local_held_weapon.weapon_name;

				if (strstr(local_held_weapon.weapon_name.c_str(), _xor_("Shotgun").c_str())) {
					gun_type = 0;
				}
				else if (strstr(local_held_weapon.weapon_name.c_str(), _xor_("Assault").c_str()) || strstr(local_held_weapon.weapon_name.c_str(), _xor_("Drum Gun").c_str()) || strstr(local_held_weapon.weapon_name.c_str(), _xor_("Peow").c_str()) || strstr(local_held_weapon.weapon_name.c_str(), _xor_("Rifle").c_str()) && !strstr(local_held_weapon.weapon_name.c_str(), _xor_("Sniper").c_str())) {
					gun_type = 1;
				}
				else if (strstr(local_held_weapon.weapon_name.c_str(), _xor_("Submachine").c_str()) || strstr(local_held_weapon.weapon_name.c_str(), _xor_("SMG").c_str()) || strstr(local_held_weapon.weapon_name.c_str(), _xor_("Minigun").c_str())) {
					gun_type = 2;
				}
				else if (strstr(local_held_weapon.weapon_name.c_str(), _xor_("Pistol").c_str()) || strstr(local_held_weapon.weapon_name.c_str(), _xor_("Pistols").c_str())) {
					gun_type = 3;
				}
				else if (strstr(local_held_weapon.weapon_name.c_str(), _xor_("Sniper").c_str())) {
					gun_type = 4;
				}
				else {
					gun_type = 5;
				}
			}
		}

		uintptr_t skeletalmesh = read<uintptr_t>(CurrentActor + offsets::Mesh);
		if (!skeletalmesh) continue;

		uintptr_t mesh = read<uintptr_t>(CurrentActor + offsets::Mesh);
		if (!skeletalmesh) continue;


		fvector base_bone = game_helper.GetBoneLocation(skeletalmesh, 0);
		if (base_bone.x == 0 && base_bone.y == 0 && base_bone.z == 0) continue;

		if (settings.chams) {



			auto Mesh = read<uint64_t>(CachePointers.AcknownledgedPawn + 0x328);
			static auto Cached = read<Vector3>(Mesh + 0x150);
			write<Vector3>(Mesh + 0x150, Vector3(0, 360, 0));
		}


		if (settings.invisible) {


			auto Mesh = read<uint64_t>(CachePointers.AcknownledgedPawn + 0x328);
			static auto Cached = read<Vector3>(Mesh + 0x139);
			write<Vector3>(Mesh + 0x139, Vector3(1, rand() % 361, 1));
		}

		if (settings.samolot) {


			//write<short>(CachePointers.AcknownledgedPawn + 0x27d4, short(257)); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerPawn&member=bHasStartedFloating
			write<short>(CachePointers.AcknownledgedPawn + 0x27e4, short(257)); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerPawn&member=bHasStartedFloating
			//write<short>(CachePointers.AcknownledgedPawn + 0x28d0, short(257)); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerPawn&member=bHasStartedFloating
			//write<short>(CachePointers.AcknownledgedPawn + 0x2888, short(257)); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerPawn&member=bHasStartedFloating
			//write<short>(CachePointers.AcknownledgedPawn + 0x27d4, short(257)); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerPawn&member=bHasStartedFloating

		}



		if (settings.samolot2) {


			if ((GetAsyncKeyState(settings.freeze_b) < 0))
			{
				write<float>(CurrentActor + 0x68, 0.0f);

			}
			else
			{
				write<float>(CurrentActor + 0x68, 1.0f); // CustomTimeDilation;
			}
		}

		if (!InScreen(game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 2)))) continue; //Pelvis

		if (CachePointers.AcknownledgedPawn) {
			auto team_index = read<int>(PlayerState + offsets::Team_Index);
			if (CachePointers.TeamIndex == team_index) continue;
		}


		if (CachePointers.AcknownledgedPawn) {
			if (settings.ignore_bots) {
				int pNameLength;
				WORD* pNameBufferPointer;
				int i;
				char v25;
				int v26;
				int v29;

				char16_t* pNameBuffer;

				uintptr_t pNameStructure = read<uintptr_t>(PlayerState + offsets::Name);
				if (is_valid(pNameStructure)) {
					pNameLength = read<int>(pNameStructure + 0x10);
					if (pNameLength <= 0)
						continue;
				}
			}
		}

		if (settings.wireframe) {
			entity actor;
			auto AllMaterials = read<TArray>((uintptr_t)CurrentActor + 0x4e70); //SDK     TArray<UMaterialInstanceDynamic> PawnMaterials_ALL;
			//Kazdy -> CurrentActor
			//Tylko Enemy - > otherPlayerPawn


			if (AllMaterials.isValid()) {
				int allMaterialsSize = AllMaterials.size();
				for (int i = 0; i < allMaterialsSize; i++) {
					auto material_instance = AllMaterials[i];
					if (material_instance != 0 && is_valid(material_instance)) {
						uintptr_t Parent = material_instance;
						for (int j = 0; j < 8; ++j) {
							if (!Parent || !is_valid(Parent)) {
								break;
							}
							Parent = read<uintptr_t>(Parent + 0x128); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UMaterialInstance&member=Parent
						}
						if (Parent && is_valid(Parent)) {
							auto disable_depth_test_ptr = (Parent + 0x1c8); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UMaterial&member=bDisableDepthTest%20:%201
							if (disable_depth_test_ptr) {
								uint8_t disable_depth_test = read<uint8_t>(disable_depth_test_ptr);
								if (disable_depth_test) {
									disable_depth_test |= (1 << 0);
									write<uint8_t>(disable_depth_test_ptr, disable_depth_test);
								}
							}
						}
						auto wireframe_ptr = (uintptr_t)(Parent + 0x1d8); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UMaterial&member=Wireframe%20:%201
						if (is_valid(wireframe_ptr)) {
							uint8_t bwireframe = read<uint8_t>(wireframe_ptr);
							if (bwireframe) {
								bwireframe |= (1 << 6);
								write<uint8_t>(wireframe_ptr, bwireframe);
							}
						}
					}
				}
			}
		}

		if (settings.FovChanger) {

			uintptr_t PlayerCameraManager = read<uintptr_t>(CachePointers.PlayerController + 0x360); //PLAYERCAMERAMANAGER 
			write<float>(PlayerCameraManager + 0x2BC + 0x4, settings.valuefov); //DEFAULTFOV + 0x4
			write(PlayerCameraManager + 0x370, settings.valuefov); //BASEFOV 

		}

		if (settings.adsinair) {

			if (CachePointers.AcknownledgedPawn) {

				write<bool>(CachePointers.AcknownledgedPawn + 0x5869, true);
			}


		}







		if (settings.humanization && settings.smoothsize < 30.0f) {
			settings.smoothsize = 30.0f;  // Ustaw na 30, jesli jest poniżej grwehw
		}





		if (settings.Fly) {


			uint64_t CurrentVehicle = read<uint64_t>(CachePointers.AcknownledgedPawn + 0x2b50); //AFortPlayerPawn::CurrentVehicle
			write<bool>(CurrentVehicle + 0x8a2, 0);
			write<bool>(CurrentVehicle + 0x8a2, 0);
		}

		if (settings.giga_bula) {
			auto size = skeletalmesh + 0x168;
			write<fvector>(size, fvector(settings.x, settings.y, settings.z));
			//https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=structs&idx=FRigUnit_DebugLine&member=B
			//https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=structs&idx=FAnimNode_AnimDynamics&member=ComponentLinearAccScale
			//https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=structs&idx=FRigUnit_HierarchyAddAnimationChannelVector2D&member=InitialValue
			//https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=structs&idx=FRigUnit_HierarchyAddAnimationChannelVector&member=InitialValue
			//https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=structs&idx=FRigUnit_HierarchyAddAnimationChannelScaleVector&member=InitialValue
			//https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=structs&idx=FRigUnit_SetRelativeTranslationForItem&member=Value
		}

		

		fvector root_bone = game_helper.GetBoneLocation(skeletalmesh, 0);
		fvector head_bone = game_helper.GetBoneLocation(skeletalmesh, 110);
		fvector2d root = game_helper.ProjectWorldToScreen(root_bone);
		fvector2d head = game_helper.ProjectWorldToScreen(head_bone);
		fvector2d root2 = game_helper.ProjectWorldToScreen(fvector(root_bone.x, root_bone.y, root_bone.z - 15));
		fvector2d head2 = game_helper.ProjectWorldToScreen(fvector(head_bone.x, head_bone.y, head_bone.z + 15));
		
		int bottom_offset = 0;
		float box_height = abs(head.y - root2.y);
		float box_width = box_height * 0.42f;
		float distance = camera_postion.location.distance(root_bone) / 100;

		static ImColor box_color;
		static ImColor filled_color;
		static ImColor skel_colror;
		static ImColor snap_colror;
		if (settings.vis_check) {
			if (game_helper.is_entity_visible(skeletalmesh))
			{
				box_color = ImColor(settings.vis[0], settings.vis[1], settings.vis[2], settings.vis[3]);
				filled_color = ImColor(settings.filledvis[0], settings.filledvis[1], settings.filledvis[2], settings.filledvis[3]);
				skel_colror = ImColor(settings.Skelvis[0], settings.Skelvis[1], settings.Skelvis[2], settings.Skelvis[3]);
				snap_colror = ImColor(settings.snapvis[0], settings.snapvis[1], settings.snapvis[2], settings.snapvis[3]);
			}
			else
			{
				box_color = ImColor(settings.invis[0], settings.invis[1], settings.invis[2], settings.invis[3]);
				filled_color = ImColor(settings.filledinvis[0], settings.filledinvis[1], settings.filledinvis[2], settings.filledinvis[3]);
				skel_colror = ImColor(settings.Skelinvis[0], settings.Skelinvis[1], settings.Skelinvis[2], settings.Skelinvis[3]);
				snap_colror = ImColor(settings.snapinvis[0], settings.snapinvis[1], settings.snapinvis[2], settings.snapinvis[3]);
			}
		}
		else {
			box_color = ImColor(settings.vis[0], settings.vis[1], settings.vis[2], settings.vis[3]);
			skel_colror = ImColor(settings.Skelvis[0], settings.Skelvis[1], settings.Skelvis[2], settings.Skelvis[3]);
			snap_colror = ImColor(settings.snapvis[0], settings.snapvis[1], settings.snapvis[2], settings.snapvis[3]);
		}
		if (settings.DeadzoneUse) {

			addToFovRadar(head_bone, ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings.draw_fov, skel_colror);
		}


		if (settings.aimbot) {
			double dx = head.x - (globals.ScreenWidth / 2);
			double dy = head.y - (globals.ScreenHeight / 2);
			float dist = sqrtf(dx * dx + dy * dy);

			if (dist < settings.fovsize && dist < TargetDistance)
			{
				TargetDistance = dist;
				TargetEntity = CurrentActor;
			}
		}

		if (settings.sigmamode) {


		}


		if (settings.font == 0) {

		}
		else if (settings.font == 1) {

		}
		else if (settings.font == 2) {

		}

		if (settings.snapline)
		{

			ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);

			ImVec2 headScreen(head.x, head.y);

			ImGui::GetBackgroundDrawList()->AddLine(screenCenter, headScreen, snap_colror);
			
		}

		if (settings.box)
		{



			const float maxDistance = 250.0f;
			const float maxThickness = 4.0f;
			const float minThickness = 0.2f;

			float thickness = maxThickness - (distance / maxDistance) * (maxThickness - minThickness);
			thickness = fmax(minThickness, thickness);

			const float halfWidth = box_width / 2.5f;
			const ImVec2 topLeft(head2.x - halfWidth, head2.y);
			const ImVec2 bottomRight(root.x + halfWidth, root.y);

			if (settings.box_outline) {
				ImGui::GetBackgroundDrawList()->AddRect(topLeft, bottomRight, ImColor(0, 0, 0), 0.0f, ImDrawFlags_RoundCornersAll, settings.thickness2 + settings.box_outline_value);
			}
			ImGui::GetBackgroundDrawList()->AddRect(topLeft, bottomRight, box_color, 0.0f, ImDrawFlags_RoundCornersAll, settings.thickness2);

			//ImGui::Image((void*)images:: baddie, ImVec2(114, 114));


		}

		if (settings.corner)
		{
			const float maxDistance = 250.0f;
			const float maxThickness = 4.0f;
			const float minThickness = 0.2f;

			float thickness = maxThickness - (distance / maxDistance) * (maxThickness - minThickness);
			thickness = fmax(minThickness, thickness);

			const float halfWidth = box_width / 2.5f;
			const ImVec2 topLeft(head2.x - halfWidth, head2.y);
			const ImVec2 bottomRight(root.x + halfWidth, root.y);

			const float shortWidth = (bottomRight.x - topLeft.x) * 0.34f;
			const float longHeight = shortWidth * 3.0f;

			ImDrawList* drawList = ImGui::GetBackgroundDrawList();


			auto DrawCornerBox = [&](const ImVec2& tl, const ImVec2& br, ImU32 color, float thickness) {

				drawList->AddLine(tl, ImVec2(tl.x + shortWidth, tl.y), color, thickness);
				drawList->AddLine(tl, ImVec2(tl.x, tl.y + longHeight), color, thickness);


				drawList->AddLine(ImVec2(br.x - shortWidth, tl.y), ImVec2(br.x, tl.y), color, thickness);
				drawList->AddLine(ImVec2(br.x, tl.y), ImVec2(br.x, tl.y + longHeight), color, thickness);


				drawList->AddLine(ImVec2(tl.x, br.y - longHeight), ImVec2(tl.x, br.y), color, thickness);
				drawList->AddLine(ImVec2(tl.x, br.y), ImVec2(tl.x + shortWidth, br.y), color, thickness);

				// Prawy dolny róg
				drawList->AddLine(ImVec2(br.x - shortWidth, br.y), ImVec2(br.x, br.y), color, thickness);
				drawList->AddLine(ImVec2(br.x, br.y - longHeight), ImVec2(br.x, br.y), color, thickness);
				};

			// Rysowanie konturu, jeśli włączony
			if (settings.box_outline) {
				DrawCornerBox(topLeft, bottomRight, ImColor(0, 0, 0), settings.thickness2 + 1.5f);
			}


			DrawCornerBox(topLeft, bottomRight, box_color, settings.thickness2);



		}

		if (settings.filled)
		{
			const float maxDistance = 250.0f;
			const float maxThickness = 3.0f;
			const float minThickness = 0.2f;

			float thickness = maxThickness - (distance / maxDistance) * (maxThickness - minThickness);
			thickness = fmax(minThickness, thickness);

			const float halfWidth = box_width / 2.5f;
			const ImVec2 topLeft(head2.x - halfWidth, head2.y);
			const ImVec2 bottomRight(root.x + halfWidth, root.y);




			float rounding = 0.0f;


			ImGui::GetBackgroundDrawList()->AddRectFilled(
				topLeft,
				bottomRight,
				filled_color,
				rounding,
				15
			);



		}
		if (settings.headesp)
		{
			const float maxDistance = 250.0f;
			const float maxThickness = 3.0f;
			const float minThickness = 0.2f;


			float distance = sqrtf(powf(head2.x - head.x, 2) + powf(head2.y - head.y, 2));


			float thickness = maxThickness - (distance / maxDistance) * (maxThickness - minThickness);
			thickness = fmax(minThickness, thickness);


			const float halfWidth = (box_width / 2.0f) * 0.3f;


			const float verticalOffset = 40.0f;
			const ImVec2 topLeft(head2.x - halfWidth, head2.y + verticalOffset - halfWidth);
			const ImVec2 bottomRight(head2.x + halfWidth, head2.y + verticalOffset + halfWidth);


			ImGui::GetBackgroundDrawList()->AddImage(
				Headesp1,
				topLeft,
				bottomRight
			);
		}





		const float spacing = 20.0f;


		float currentYPosition = root.y;


		if (settings.platform) {
			ImVec2 textPosition(root.x, currentYPosition);
			std::string PlatformName = get_platform(PlayerState);


			if (strstr(PlatformName.c_str(), _xor_("WIN").c_str())) {
				Draw_Text("WIN", textPosition, settings.size_font, ImColor(settings.paltform[0], settings.paltform[1], settings.paltform[2], settings.paltform[3]));
			}
			else if (strstr(PlatformName.c_str(), _xor_("PSN").c_str()) || strstr(PlatformName.c_str(), _xor_("PS5").c_str())) {
				Draw_Text("PS5", textPosition, settings.size_font, ImColor(settings.paltform[0], settings.paltform[1], settings.paltform[2], settings.paltform[3]));
			}
			else if (strstr(PlatformName.c_str(), _xor_("XBL").c_str()) || strstr(PlatformName.c_str(), _xor_("XSX").c_str())) {
				Draw_Text("XBOX", textPosition, settings.size_font, ImColor(settings.paltform[0], settings.paltform[1], settings.paltform[2], settings.paltform[3]));
			}
			else if (strstr(PlatformName.c_str(), _xor_("SWT").c_str())) {
				Draw_Text("NINTENDO", textPosition, settings.size_font, ImColor(settings.paltform[0], settings.paltform[1], settings.paltform[2], settings.paltform[3]));
			}
			else {
				Draw_Text(PlatformName.c_str(), textPosition, settings.size_font, ImColor(settings.paltform[0], settings.paltform[1], settings.paltform[2], settings.paltform[3]));
			}


			currentYPosition += spacing;
		}

		if (settings.rankesp) {

			ImVec2 textPosition(head2.x, head2.y - 10);

			int32_t rank_progress = read<int32_t>(read<uintptr_t>(PlayerState + 0xa48) + 0xa48 + 0x10);


			char buffer[128];
			sprintf_s(buffer, hash_string("%s"), RankedProgress.c_str());
			Draw_Text(buffer, textPosition, rank_progress, settings.size_font);

		}






		if (settings.dist) {
			ImVec2 textPosition(root.x, currentYPosition);
			std::string distance_str = std::to_string(static_cast<int>(distance)) + ("m");
			char buffer[128];
			sprintf_s(buffer, hash_string("%s"), distance_str.c_str());
			Draw_Text(buffer, textPosition, settings.size_font, ImColor(settings.distance[0], settings.distance[1], settings.distance[2], settings.distance[3]));

			currentYPosition += spacing;
		}




		if (settings.Radar) {

			render_radar_main();
			add_to_radar(root_bone, distance, skeletalmesh);
		}


		if (settings.arrow_fov) {


		}


		if (settings.adsinair) {

			if (CachePointers.AcknownledgedPawn)
			{
				write<bool>(CachePointers.AcknownledgedPawn + offsets::b_ads_while_not_on_ground, true);
			}
		}
		
		
		if (settings.skel)
		{
			// Bone positions
			fvector2d bonePositions[19];
			bonePositions[0] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 110));  // Head
			bonePositions[1] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 66));   // Neck
			bonePositions[2] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 7));    // Chest
			bonePositions[3] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 2));    // Pelvis
			bonePositions[4] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 9));    // Right Shoulder
			bonePositions[5] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 10));   // Right Elbow
			bonePositions[6] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 11));   // Right Wrist
			bonePositions[7] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 38));   // Left Shoulder
			bonePositions[8] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 39));   // Left Elbow
			bonePositions[9] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 40));   // Left Wrist
			bonePositions[10] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 71));  // Right Hip
			bonePositions[11] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 72));  // Right Knee
			bonePositions[12] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 73));  // Right Ankle
			bonePositions[13] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 86));  // Right Foot Upper
			bonePositions[14] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 76));  // Right Foot
			bonePositions[15] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 78));  // Left Hip
			bonePositions[16] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 79));  // Left Knee
			bonePositions[17] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 80));  // Left Ankle
			bonePositions[18] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 87));  // Left Foot Upper
			bonePositions[19] = game_helper.ProjectWorldToScreen(game_helper.GetBoneLocation(skeletalmesh, 83));  // Left Foot

			// Bone pairs for the skeleton
			std::vector<std::pair<int, int>> bonePairs;

			if (settings.skel)
			{
				bonePairs = {
					{1, 1},  // 0 -> Szyja (Neck) -> Szyja (Neck)
					{1, 2},  // 1 -> Szyja (Neck) -> Klatka piersiowa (Chest)
					{2, 3},  // 2 -> Klatka piersiowa (Chest) -> Miednica (Pelvis)
					{7, 4},  // 2 -> Klatka Piersiow (Chest) -> Prawe ramię (Right Shoulder)
					{4, 5},  // 4 -> Prawe ramię (Right Shoulder) -> Prawe łokieć (Right Elbow)
					{5, 6},  // 5 -> Prawe łokieć (Right Elbow) -> Prawe nadgarstek (Right Wrist)
					{4, 7},  // 2 -> Klatka Piersiowa (Chest) -> Lewe ramię (Left Shoulder)
					{7, 8},  // 7 -> Lewe ramię (Left Shoulder) -> Lewy łokieć (Left Elbow)
					{8, 9},  // 8 -> Lewy łokieć (Left Elbow) -> Lewy nadgarstek (Left Wrist)
					{3, 10}, // 3 -> Miednica (Pelvis) -> Prawe biodro (Right Hip)
					{10, 12},// 11 -> Prawe biodro (Right Hip) -> Prawe kolano (Right Knee)
					{12, 13},// 12 -> Prawe kolano (Right Knee) -> Prawe kostka (Right Ankle)

					{3, 15}, // 3 -> Miednica (Pelvis) -> Lewe biodro (Left Hip)
					{15, 17},// 16 -> Lewe biodro (Left Hip) -> Lewe kolano (Left Knee)
					{17, 18},// 17 -> Lewe kolano (Left Knee) -> Lewe kostka (Left Ankle)

				};
			}

			if (settings.skel2)
			{
				bonePairs = {
					{0, 1},  // 0 -> Szyja (Neck) -> Szyja (Neck)
					{1, 2},  // 1 -> Szyja (Neck) -> Klatka piersiowa (Chest)
					{2, 3},  // 2 -> Klatka piersiowa (Chest) -> Miednica (Pelvis)
					{7, 4},  // 2 -> Klatka Piersiow (Chest) -> Prawe ramię (Right Shoulder)
					{4, 5},  // 4 -> Prawe ramię (Right Shoulder) -> Prawe łokieć (Right Elbow)
					{5, 6},  // 5 -> Prawe łokieć (Right Elbow) -> Prawe nadgarstek (Right Wrist)
					{4, 7},  // 2 -> Klatka Piersiowa (Chest) -> Lewe ramię (Left Shoulder)
					{7, 8},  // 7 -> Lewe ramię (Left Shoulder) -> Lewy łokieć (Left Elbow)
					{8, 9},  // 8 -> Lewy łokieć (Left Elbow) -> Lewy nadgarstek (Left Wrist)
					{3, 11}, // 3 -> Miednica (Pelvis) -> Prawe biodro (Right Hip)
					{11, 12},// 11 -> Prawe biodro (Right Hip) -> Prawe kolano (Right Knee)
					{12, 13},// 12 -> Prawe kolano (Right Knee) -> Prawe kostka (Right Ankle)

					{3, 16}, // 3 -> Miednica (Pelvis) -> Lewe biodro (Left Hip)
					{16, 17},// 16 -> Lewe biodro (Left Hip) -> Lewe kolano (Left Knee)
					{17, 18},// 17 -> Lewe kolano (Left Knee) -> Lewe kostka (Left Ankle)
				};
			}

			if (settings.skel3)
			{
				bonePairs = {
					{0, 1},  // 0 -> Głowa (Head) -> Szyja (Neck)
					{1, 2},  // 1 -> Szyja (Neck) -> Klatka piersiowa (Chest)
					{2, 3},  // 2 -> Klatka piersiowa (Chest) -> Miednica (Pelvis)
					{1, 4},  // 2 -> Szyja (Neck) -> Prawe ramię (Right Shoulder)
					{4, 5},  // 4 -> Prawe ramię (Right Shoulder) -> Prawe łokieć (Right Elbow)
					{5, 6},  // 5 -> Prawe łokieć (Right Elbow) -> Prawe nadgarstek (Right Wrist)
					{1, 7},  // 2 -> Szyja (Neck) -> Lewe ramię (Left Shoulder)
					{7, 8},  // 7 -> Lewe ramię (Left Shoulder) -> Lewy łokieć (Left Elbow)
					{8, 9},  // 8 -> Lewy łokieć (Left Elbow) -> Lewy nadgarstek (Left Wrist)
					{3, 10}, // 3 -> Miednica (Pelvis) -> Prawe biodro (Right Hip)
					{10, 11},// 10 -> Prawe biodro (Right Hip) -> Prawe kolano (Right Knee)
					{11, 12},// 11 -> Prawe kolano (Right Knee) -> Prawe kostka (Right Ankle)
					{12, 13},// 12 -> Prawe kostka (Right Ankle) -> Górna część prawej stopy (Right Foot Upper)

					{3, 15}, // 3 -> Miednica (Pelvis) -> Lewe biodro (Left Hip)
					{15, 16},// 15 -> Lewe biodro (Left Hip) -> Lewe kolano (Left Knee)
					{16, 17},// 16 -> Lewe kolano (Left Knee) -> Lewe kostka (Left Ankle)
					{17, 18},// 17 -> Lewe kostka (Left Ankle) -> Górna część lewej stopy (Left Foot Upper)

				};
			}
			ImDrawList* Draw = ImGui::GetBackgroundDrawList();

			// Draw the skeleton
			ImGui::GetBackgroundDrawList()->PushClipRectFullScreen();
			for (const auto& pair : bonePairs)
			{
				ImVec2 start(bonePositions[pair.first].x, bonePositions[pair.first].y);
				ImVec2 end(bonePositions[pair.second].x, bonePositions[pair.second].y);
				uintptr_t hello = reinterpret_cast<uintptr_t>(&start); // ✅ pointer cast
				//DrawSkeleton(hello, color, true, false);
				if (settings.skel_outline)
				{
					ImGui::GetBackgroundDrawList()->AddLine(start, end, ImColor(0, 0, 0), settings.box_outline_value + settings.SkelThickness);
				}
				

					//ImGui::GetBackgroundDrawList()->AddLine(start, end, skel_colror, settings.SkelThickness);
				ImGui::GetBackgroundDrawList()->AddLine(start, end, skel_colror, settings.SkelThickness);
			}
			ImGui::GetBackgroundDrawList()->PopClipRect();
		}


		if (settings.weapon_hold) {
			ImVec2 textPosition(root.x, currentYPosition);
			weapon_info_ local_held_weapon = get_weapon_name(CurrentActor);
			LocalPlayerWeapon = local_held_weapon.weapon_name;

			char buffer[128];
			sprintf_s(buffer, hash_string("%s"), LocalPlayerWeapon.c_str());
			Draw_Text(buffer, textPosition, settings.size_font, ImColor(255, 255, 255, 255));
			currentYPosition += spacing;



		}

		if (settings.user) {
			//ImVec2 textPosition(root.x, currentYPosition); 
			ImVec2 textPosition(head2.x, head2.y - 20); // anthign else nigga  /// / / // /  //  . .  . //  / / sex
			std::string playerName = decryption.GetPlayerName(PlayerState);
			char buffer[128];
			sprintf_s(buffer, hash_string("%s"), playerName.c_str());
			Draw_Text(buffer, textPosition, settings.size_font, ImColor(settings.username[0], settings.username[1], settings.username[2], settings.username[3]));

			// Zwiększenie pozycji Y o stal y odstęp
			//currentYPosition += spacing;
		}

		if (settings.trigger && (GetAsyncKeyState(settings.trig_b) < 0))
		{
			weapon_info_ local_held_weapon = get_weapon_name(CachePointers.AcknownledgedPawn);
			LocalPlayerWeapon = local_held_weapon.weapon_name;

			if (settings.only_pump) {
				if (strstr(local_held_weapon.weapon_name.c_str(), _xor_("Pump Shotgun").c_str()) ||
					strstr(local_held_weapon.weapon_name.c_str(), _xor_("Pump").c_str()) ||
					strstr(local_held_weapon.weapon_name.c_str(), _xor_("Shotgun").c_str()) ||
					strstr(local_held_weapon.weapon_name.c_str(), _xor_("Strzelba").c_str())) {

					if (read<uintptr_t>(CachePointers.PlayerController + offsets::TargetedFortPawn))
					{
						if (distance <= settings.t_distance)
						{

							utils.left_click();
						}
					}
				}
			}
			else {
				if (read<uintptr_t>(CachePointers.PlayerController + offsets::TargetedFortPawn))
				{
					if (distance <= settings.t_distance)
					{

						utils.left_click();
					}
				}
			}
		}

		if (settings.font == 0) {

		}
		else {
			//ImGui::PopFont();
		}

		entity cached_actors{ };
		cached_actors.entity = CurrentActor;
		cached_actors.skeletal_mesh = skeletalmesh;
		cached_actors.player_state = PlayerState;
		temporary_entity_list.push_back(cached_actors);
	}



	if (TargetEntity && settings.aimbot) {
		auto ClosestMesh = read<uint64_t>(TargetEntity + offsets::Mesh);
		game_helper.get_view_point();
		//game_helper.get_camera();
		fvector Hitbox = game_helper.GetBoneLocation(ClosestMesh, 66);

		float smooth;
		float fov;
		int kurwa_bone;

		if (settings.wpn_cfg) {

			if (gun_type == 0) {
				smooth = weapons.smooth_shotgun;
				fov = weapons.fov_shotgun;
				kurwa_bone = weapons.bone_shotgun;
			}
			else if (gun_type == 1) {
				smooth = weapons.smooth_rifle;
				fov = weapons.fov_rifle;
				kurwa_bone = weapons.bone_rifle;
			}
			else if (gun_type == 2) {
				smooth = weapons.smooth_smgs;
				fov = weapons.fov_smgs;
				kurwa_bone = weapons.bone_smgs;
			}
			else if (gun_type == 3) {
				smooth = weapons.smooth_pistols;
				fov = weapons.fov_pistols;
				kurwa_bone = weapons.bone_pistols;
			}
			else if (gun_type == 4) {
				smooth = weapons.smooth_snipers;
				fov = weapons.fov_snipers;
				kurwa_bone = weapons.bone_snipers;
			}
			else if (gun_type == 5) {
				smooth = weapons.smooth_others;
				fov = weapons.fov_others;
				kurwa_bone = weapons.bone_others;
			}
		}
		else
		{
			smooth = settings.smoothsize;
			fov = settings.fovsize;
			kurwa_bone = settings.bone;

		}

		if (kurwa_bone == 0)
		{
			Hitbox = game_helper.GetBoneLocation(ClosestMesh, 110);
		}
		else if (kurwa_bone == 1)
		{
			Hitbox = game_helper.GetBoneLocation(ClosestMesh, 66);
		}

		fvector2d HitboxScreen = game_helper.ProjectWorldToScreen(Hitbox);
		auto distance = camera_postion.location.distance(Hitbox);
		float distance2 = distance / 100.0f;

		if (HitboxScreen.x != 0 || HitboxScreen.y != 0) {
			float crossDist = get_cross_distance(HitboxScreen.x, HitboxScreen.y, globals.ScreenWidth / 2, globals.ScreenHeight / 2);

			if (crossDist <= fov) {
				if (settings.vis_check) {
					if (game_helper.is_entity_visible(ClosestMesh)) {
						if (GetAsyncKeyState(settings.aim_b)) {
							if (settings.predict) {
								uintptr_t currentweapon = read<uintptr_t>(CachePointers.AcknownledgedPawn + offsets::CurrentWeapon);
								float projectileSpeed = read<float>(currentweapon + offsets::speed);
								float projectileGravityScale = read<float>(currentweapon + offsets::gravity);

								auto root = read<uintptr_t>(TargetEntity + offsets::RootComponent);
								fvector velocity = read<fvector>(root + offsets::Velocity);

								if (projectileSpeed > 1000) {
									Hitbox = game_helper.PredictLocation(Hitbox, velocity, projectileSpeed, projectileGravityScale, distance);
								}

								HitboxScreen = game_helper.ProjectWorldToScreen(Hitbox);
								if (settings.aimbot2)
								{

									float projectileSpeed = 30000;
									float projectileGravityScale = 6.0f;

									move(HitboxScreen, smooth);
								}
								else if (settings.Memoryaim)
								{


									float projectileSpeed = 30000;
									float projectileGravityScale = 6.0f;

									memoryMove(HitboxScreen, smooth);
								}
							}
							else {
								HitboxScreen = game_helper.ProjectWorldToScreen(Hitbox);
								if (settings.aimbot2)
								{
									move(HitboxScreen, smooth);
								}
								else if (settings.Memoryaim)
								{
									memoryMove(HitboxScreen, smooth);
								}
							}
						}
					}
				}
				else {
					if (settings.predict) {
						uintptr_t currentweapon = read<uintptr_t>(CachePointers.AcknownledgedPawn + offsets::CurrentWeapon);
						float projectileSpeed = read<float>(currentweapon + offsets::speed);
						float projectileGravityScale = read<float>(currentweapon + offsets::gravity);

						auto root = read<uintptr_t>(TargetEntity + offsets::RootComponent);
						fvector velocity = read<fvector>(root + offsets::Velocity);

						if (projectileSpeed > 1000) {
							Hitbox = game_helper.PredictLocation(Hitbox, velocity, projectileSpeed, projectileGravityScale, distance);
						}

						HitboxScreen = game_helper.ProjectWorldToScreen(Hitbox);
						if (settings.aimbot2)
						{
							move(HitboxScreen, smooth);
						}
						else if (settings.Memoryaim)
						{
							memoryMove(HitboxScreen, smooth);
						}

					}
					else {
						HitboxScreen = game_helper.ProjectWorldToScreen(Hitbox);
						if (settings.aimbot2)
						{
							move(HitboxScreen, smooth);
						}
						else if (settings.Memoryaim)
						{
							memoryMove(HitboxScreen, smooth);
						}
					}
				}
			}
		}
	}
	else
	{
		TargetDistance = FLT_MAX;
		TargetEntity = NULL;
	}

	entity_list.clear();
	entity_list = temporary_entity_list;
}
