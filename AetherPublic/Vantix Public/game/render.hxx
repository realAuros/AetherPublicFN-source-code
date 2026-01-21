// render.hxx
#pragma once
#include <d3d11.h>
#include <cfg.h>
#include "imgui/imgui.h"
#include "settings.hxx"
#include "../backend/imgui_impl_dx11.h"
#include "../backend/imgui_impl_win32.h"

void render();
extern unsigned char Font[];
extern unsigned char Font1[];
extern unsigned char icon[];
extern unsigned char icon2[];
extern unsigned char iconc[];
extern unsigned char dragon[];

extern ID3D11Device* d3d_device;
extern ID3D11DeviceContext* d3d_device_ctx;
extern IDXGISwapChain* swap_chain;
extern ID3D11RenderTargetView* target_view;
extern IDXGISwapChain* d3d_swap_chain;

extern class globals_t globals;
extern class settings_t settings;
extern class weapons_t weapons;

extern ID3D11RenderTargetView* d3d_render_target;
extern ImVec4 accentcolor;

namespace fonts {
    extern ImFont* font;
    extern ImFont* font2;
    extern ImFont* Main_Font;
    extern ImFont* Regylar;
    extern ImFont* icons;
    extern ImFont* ico_list;
    extern ImFont* ico;
}

class overlay_t {
public:
    bool initialize_overlay();
    bool initialize_imgui();
    void LoadStyles();
    void draw_instance();
    void overlay_drawlist();
    void cleanup_render_target();
    void create_render_target();
    void shutdown();
    void toggle_clickability(bool enable);
    void thread();
    static LRESULT CALLBACK static_wnd_proc_hook(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
    LRESULT CALLBACK wnd_proc_hook(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
    DWORD WINAPI render_loop();
    bool is_fortnite_active();
};

extern overlay_t overlay;
