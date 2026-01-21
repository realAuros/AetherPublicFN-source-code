#include <wtypes.h>
#ifndef SETTINGS_H
#define SETTINGS_H
#include <d3d11.h>
#include <windows.h>
#include <dwmapi.h>
#include <string>
#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")
#include <mutex>
#include <memory>
#include <string>

#define loadername = "VANTIX"

class globals_t
{
public:
    int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int ScreenHeight = GetSystemMetrics(SM_CXSCREEN);

    __int64 va_text = 0;
    HWND window_handle;
    std::string random;
};

// Deklaracja zewn.
extern globals_t globals;

namespace images {
    inline ID3D11ShaderResourceView* baddie = nullptr;
    inline ID3D11ShaderResourceView* nude = nullptr;
    inline ID3D11ShaderResourceView* sophie = nullptr;
    inline ID3D11ShaderResourceView* gyatt = nullptr;

}

class settings_t
{
public:
    bool traceblocker = true;

    bool aimbot = true;
    bool silentaim = false;
    bool aimbot2 = false;
    bool adsinair = false;
    bool Memoryaim = true;
    bool CloseAim = false;
    bool AdvancedAim = false;
    bool ControllerSupport = true;
    bool DeadzoneUse = false;
    bool humanization = false;
    bool silent = false;
    bool wpn_cfg = false;
    bool predict = false;
    int bone = 0;

    bool chinesehat = false;

    float TargetFps = 240;
    int Closebone = 0;
    float Closefovsize = 150;
    bool CloseDrawFov = false;
    float Closesmoothsize = 6;
    float fovsize = 160;

       const char* bones[3] = { "Head", "Neck", "Chest" };

       bool multi_bone[3];

       

    // custom fov
    int fovstyle = 0;
    const char* fovstylchar[4] = { "Static", "Breathing", "Pulsing", "Animated dots" };
    // end of custom 

    float thickness2 = 1.5;
    float box_outline_value = 1.0f;
    float thickness3 = 1.5;
    bool chestesp = false;
    bool enableworld = false;
    bool ammobox = false;
    float  maxdist = 80.f;
    float smoothsize = 6;
    float SmoothX = 6;
    float SmoothY = 6;

    bool trigger = false;
    bool only_pump = true;
    float t_distance = 16;

    bool vis_check = true;

    int guns = 0;

    int font = 0;

    bool FovChanger = false;
    float valuefov = 0;

    bool wireframe = false;    float size_font = 15.f;

    bool chams = false;
    bool invisible = false;
    float deadzone = 0;
    bool samolot = false;
    bool samolot2 = false;
    int prendkosc = 3;


    float Boxvis[4] = { 1.f, 1.f, 1.f, 1.f };
    float Boxinvis[4] = { 1.f, 1.f, 1.f, 1.f };
    float filledvis[4] = { 25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 70.f / 255.f }; //this is like startup color you will have on esp wow :D
    float filledinvis[4] = { 1.f, 1.f, 1.f, 140.f / 255.f };
    float Skelvis[4] = { 1.f, 1.f, 1.f, 1.f };
    float snapvis[4] = { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 140.f / 255.f };
    float snapinvis[4] = { 1.f, 1.f, 1.f, 140.f / 255.f };
    float Skelinvis[4] = { 1.f, 1.f, 1.f, 1.f };
    float vis[4] = { 1.f, 1.f, 1.f, 1.f };
    float invis[4] = { 1.f, 1.f, 1.f, 1.f };
    float username[4] = { 1.f, 1.f, 1.f, 1.f };
    float distance[4] = { 1.f, 1.f, 1.f, 1.f };
    float paltform[4] = { 1.f, 1.f, 1.f, 1.f };
    float fov[4] = { 1.f, 1.f, 1.f, 1.f };

    /* IMAGES DEFS */
    bool sigmamode = false;
    /* END OF IMAGE DEFS */

    /* CUSTOM STUFF */
    bool baddie = false;
    bool gyatt = false;
    bool sophie = false;
    bool nude = false;
    /* END OF CUSTOM STUFF */


    bool box = false;
    bool snapline = false;
    bool arrow_fov = false;
    float radar_range = 100.f;
    bool corner = false;
    bool headesp = false;
    bool filled = false;
    int alpha = 190;
    float radar_pos_y = 300;
    float radar_pos_x = 300;
    float radar_size = 250;
    bool user = false;
    bool ammo = false;
    bool dist = false;
    bool Radar = false;
    bool fovreset = false;
    bool rankesp = false;
    bool platform = false;
    bool skel = false;
    float SkelThickness = 1.7;
    bool skel1 = true;
    bool skel2 = false;
    bool skel3 = false;
    bool skel_outline = false;
    bool box_outline = false;

    bool weapon_hold = false;
    bool ignore_bots = false;

    bool draw_fov = false;
    bool crosshair = false;
    bool fill_fov = false;

    bool optimization = false;
    bool Fly = false;
    bool giga_bula = false;
    float x = 15;
    float y = 1.75;
    float z = 1.5;
    float flick_speed = 500;
    bool stream = false;
    bool vsync = true;
    float fpscap = 1000;
    int aim_b = VK_MENU;
    int freeze_b = VK_RBUTTON;
    int trig_b = VK_RBUTTON;
};

// Deklaracja zewnętrzna
extern settings_t settings;

class weapons_t
{
public:

    float smooth_shotgun = 5;
    float fov_shotgun = 110;
    int bone_shotgun = 0;

    float smooth_rifle = 5;
    float fov_rifle = 110;
    int bone_rifle = 1;

    float smooth_smgs = 5;
    float fov_smgs = 110;
    int bone_smgs = 1;

    float smooth_pistols = 5;
    float fov_pistols = 110;
    int bone_pistols = 1;

    float smooth_snipers = 5;
    float fov_snipers = 110;
    int bone_snipers = 0;

    float smooth_others = 5;
    float fov_others = 110;
    int bone_others = 0;
};

extern weapons_t weapons;

#endif // SETTINGS_H