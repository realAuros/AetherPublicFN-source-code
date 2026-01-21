
#include "render.hxx"
#include "gui.h"
#include "../framework/Font.h"
#include "special.h"


ID3D11Device* d3d_device = NULL;
ID3D11DeviceContext* d3d_device_ctx = NULL;
IDXGISwapChain* swap_chain = NULL;
ID3D11RenderTargetView* target_view = NULL;
IDXGISwapChain* d3d_swap_chain;
globals_t globals;
settings_t settings;
weapons_t weapons;
ID3D11RenderTargetView* d3d_render_target;
ImVec4 accentcolor = ImColor(88, 192, 228);

namespace fonts {
    ImFont* font = nullptr;
    ImFont* font2 = nullptr;
    ImFont* Main_Font = nullptr;
    ImFont* Regylar = nullptr;
    ImFont* icons = nullptr;
    ImFont* ico_list = nullptr;
    ImFont* ico = nullptr;
}

class glob_t {
public:
    HWND window;
};
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);

HWND game_window;
RECT game_rec;
int gamex, gamey, gamewidth, gameheight, overlaywidth, overlayheight;

void overlay_t::thread() {
    std::thread(niga).detach();
}

auto overlay_t::LoadStyles() -> void
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGuiStyle* iStyle = &ImGui::GetStyle();
        ImGuiStyle& style = ImGui::GetStyle();


        style.Colors[ImGuiCol_WindowBg] = ImColor(12, 12, 12);
        style.Colors[ImGuiCol_ChildBg] = ImColor(9, 9, 9);
        style.Colors[ImGuiCol_Border] = ImColor(12, 12, 12, 0);
        style.Colors[ImGuiCol_Button] = ImColor(88, 192, 228);
        style.Colors[ImGuiCol_ButtonHovered] = ImColor(60, 150, 190);
        style.Colors[ImGuiCol_ButtonActive] = ImColor(90, 170, 200);

        style.ChildRounding = 5.f;
        style.ItemSpacing = ImVec2(0, 0);
        ImGui::GetStyle().WindowPadding = ImVec2(0, 0);
        style.WindowRounding = 5.f;

        static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
        ImFontConfig icons_config;
        ImFontConfig CustomFont;
        CustomFont.FontDataOwnedByAtlas = false;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.OversampleH = 3;
        icons_config.OversampleV = 3;

        io.IniFilename = NULL;
        io.LogFilename = NULL;
        ImFontConfig cfg;
        fonts::font = io.Fonts->AddFontFromMemoryTTF(&Font, sizeof Font, 24.f, NULL, io.Fonts->GetGlyphRangesCyrillic());
        fonts::font2 = io.Fonts->AddFontFromMemoryTTF(&Font, sizeof Font, 18.f, NULL, io.Fonts->GetGlyphRangesCyrillic());
        fonts::Main_Font = io.Fonts->AddFontFromMemoryTTF(&Font, sizeof Font, 18.f, NULL, io.Fonts->GetGlyphRangesCyrillic());
        fonts::Regylar = io.Fonts->AddFontFromMemoryTTF(&Font1, sizeof Font1, 18.f, NULL, io.Fonts->GetGlyphRangesCyrillic());
        fonts::icons = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 28, NULL, io.Fonts->GetGlyphRangesCyrillic());
        fonts::ico_list = io.Fonts->AddFontFromMemoryTTF(&icon2, sizeof(icon2), 18, NULL, io.Fonts->GetGlyphRangesCyrillic());
        fonts::ico = io.Fonts->AddFontFromMemoryTTF(&iconc, sizeof(iconc), 26, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }

bool overlay_t::initialize_imgui()
    {
        DXGI_SWAP_CHAIN_DESC swap_chain_description;
        ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
        swap_chain_description.BufferCount = 2;
        swap_chain_description.BufferDesc.Width = 0;
        swap_chain_description.BufferDesc.Height = 0;
        swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_description.BufferDesc.RefreshRate.Numerator = 240;
        swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
        swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_description.OutputWindow = globals.window_handle;
        swap_chain_description.SampleDesc.Count = 1;
        swap_chain_description.SampleDesc.Quality = 0;
        swap_chain_description.Windowed = 1;
        swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL d3d_feature_lvl;

        const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

        D3D11CreateDeviceAndSwapChain(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            0,
            d3d_feature_array,
            2,
            D3D11_SDK_VERSION,
            &swap_chain_description,
            &swap_chain, &d3d_device,
            &d3d_feature_lvl,
            &d3d_device_ctx
        );

        ID3D11Texture2D* pBackBuffer;
        swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &target_view);
        pBackBuffer->Release();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        io.IniFilename = NULL;
        io.LogFilename = NULL;

        ImGui_ImplWin32_Init(globals.window_handle);
        ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

        ImFontConfig font_config;
        font_config.OversampleH = 1;
        font_config.OversampleV = 1;
        font_config.PixelSnapH = 1;

        static const ImWchar ranges[] = {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF, // Cyrillic Extended-A
            0xA640, 0xA69F, // Cyrillic Extended-B
            0xE000, 0xE226, // icons
            0,
        };
        font_config.GlyphRanges = ranges;

        this->LoadStyles();

        d3d_device->Release();
        return true;
    }

void overlay_t::cleanup_render_target() {
        if (target_view) {
            target_view->Release();
            target_view = NULL;
        }
    }

void overlay_t::create_render_target() {
        ID3D11Texture2D* back_buffer;
        swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
        d3d_device->CreateRenderTargetView(back_buffer, NULL, &target_view);
        back_buffer->Release();
    }

void overlay_t::shutdown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        PostQuitMessage(0);
        exit(0x100);
    }

void overlay_t::toggle_clickability(bool enable) {
        LONG style = GetWindowLongA(globals.window_handle, GWL_EXSTYLE);

        if (enable) {
            style &= ~WS_EX_TRANSPARENT; // Zdejmowanie transparentności
            style &= ~WS_EX_LAYERED; // Umożliwienie normalnego działania okna
        }
        else {
            style |= WS_EX_LAYERED; // Dodanie przezroczystości
            style |= WS_EX_TRANSPARENT; // Odbieranie kliknięć
        }

        SetWindowLongA(globals.window_handle, GWL_EXSTYLE, style);
    }

LRESULT CALLBACK overlay_t::static_wnd_proc_hook(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
        overlay_t* overlay = reinterpret_cast<overlay_t*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (overlay) {
            return overlay->wnd_proc_hook(hwnd, message, w_param, l_param);
        }
        return DefWindowProc(hwnd, message, w_param, l_param);
    }

LRESULT CALLBACK overlay_t::wnd_proc_hook(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
        switch (message) {
        case WM_DESTROY:
            this->shutdown();
            break;
        case WM_SIZE:
            if (d3d_device != NULL && w_param != SIZE_MINIMIZED) {
                this->cleanup_render_target();
                swap_chain->ResizeBuffers(0, (UINT)LOWORD(l_param), (UINT)HIWORD(l_param), DXGI_FORMAT_UNKNOWN, 0);
                this->create_render_target();
            }
            break;
        }
        return DefWindowProc(hwnd, message, w_param, l_param);
    }

bool overlay_t::initialize_overlay() {

        int screenWidth = globals.ScreenWidth;
        int screenHeight = globals.ScreenHeight;

        const char* ClassName = ("Fortnite");

        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = static_wnd_proc_hook;
        wc.lpszClassName = ClassName;
        (RegisterClassExA)(&wc);

        globals.window_handle = (CreateWindowExA)(
            (DWORD)NULL,
            ClassName,
            ClassName,
            WS_POPUP | WS_VISIBLE,
            0, 0, globals.ScreenWidth, globals.ScreenHeight,
            (HWND)0, (HMENU)0, (HINSTANCE)0, (LPVOID)0);

        if (!globals.window_handle)
            return FALSE;

        (SetWindowLongA)(globals.window_handle, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);

        MARGINS Margin = { -1 };
        DwmExtendFrameIntoClientArea(globals.window_handle, &Margin);
        (ShowWindow)(globals.window_handle, SW_SHOW);
        (UpdateWindow)(globals.window_handle);

        if (!globals.window_handle)
            return FALSE;

        return TRUE;
    }


void overlay_t::overlay_drawlist() {
        if (settings.draw_fov) {

            int size = settings.fovsize;


            ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2), size, ImColor(255, 255, 255, 255), 128, 1.2f);
        }
        if (settings.fill_fov) {
            int fill_size;

            fill_size = settings.fovsize;


            // Dodanie szarego wypełnienia FOV
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2), fill_size, ImColor(25, 25, 25, 70)); // Użycie szarego koloru (200, 200, 200) z przezroczystością 128
        }

        if (settings.CloseDrawFov) {
            ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2), settings.Closefovsize, ImColor(255, 255, 255, 255), 128, 1.2f);
        }

        if (settings.crosshair) {
            // Left
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(globals.ScreenWidth / 2 - 7, globals.ScreenHeight / 2), ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2), ImColor(255, 255, 255, 255), 1.9f);
            // Right // change it idiot
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(globals.ScreenWidth / 2 + 8, globals.ScreenHeight / 2), ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2), ImColor(255, 255, 255, 255), 1.9f);

            // Top
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2 - 7), ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2), ImColor(255, 255, 255, 255), 1.9f);
            // Bottom
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2 + 8), ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2), ImColor(255, 255, 255, 255), 1.9f);
        }
    }




void overlay_t::draw_instance()
    {


        static bool showed = true;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        actorloop();
        overlay_drawlist();

        if (settings.stream) {
            SetWindowDisplayAffinity(globals.window_handle, WDA_EXCLUDEFROMCAPTURE);
        }
        else {
            SetWindowDisplayAffinity(globals.window_handle, !WDA_EXCLUDEFROMCAPTURE);
        }

        if (settings.vsync) {

            swap_chain->Present(1, 0);



        }
        else {


            swap_chain->Present(0, 0);

        }

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            showed = !showed;
        }

        if (showed) {

            render();

            ImVec2 cursor_pos = ImGui::GetMousePos();
            ImGui::GetForegroundDrawList()->AddCircleFilled(cursor_pos, 3.f, ImColor(255, 255, 255, 255), 14);

            toggle_clickability(true); // Gdy overlay jest widoczny i interaktywny
        }
        else {
            toggle_clickability(false); // Gdy overlay jest ukryty
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        d3d_device_ctx->OMSetRenderTargets(1, &target_view, nullptr);
        d3d_device_ctx->ClearRenderTargetView(target_view, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



    }

bool overlay_t::is_fortnite_active() {
        HWND hwnd = GetForegroundWindow();
        if (hwnd) {
            char class_name[256];
            GetClassNameA(hwnd, class_name, sizeof(class_name));
            std::string current_class_name = class_name;
            return current_class_name == "UnrealWindow";
        }
        return false;
    }

DWORD WINAPI overlay_t::render_loop() {
        RECT old_rc = { 0, 0, 0, 0 };
        MSG Message = { NULL };
        ZeroMemory(&Message, sizeof(MSG));

        while (Message.message != WM_QUIT) {
            if (PeekMessage(&Message, globals.window_handle, 0, 0, PM_REMOVE)) {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }

            game_window = FindWindowA("UnrealWindow", "Fortnite  ");
            if (GetWindow(game_window, 0) == NULL) {
                SendMessage(globals.window_handle, WM_DESTROY, 0, 0);
                break;
            }

            HWND hwnd_active = GetForegroundWindow();
            if (hwnd_active == game_window || hwnd_active == globals.window_handle) {
                POINT xy;
                ZeroMemory(&game_rec, sizeof(RECT));
                ZeroMemory(&xy, sizeof(POINT));

                GetClientRect(game_window, &game_rec);
                ClientToScreen(game_window, &xy);
                game_rec.left = xy.x;
                game_rec.top = xy.y;

                if (game_rec.left != old_rc.left || game_rec.right != old_rc.right ||
                    game_rec.top != old_rc.top || game_rec.bottom != old_rc.bottom) {
                    old_rc = game_rec;
                    gamex = game_rec.left;
                    gamey = game_rec.top;
                    gamewidth = game_rec.right;
                    gameheight = game_rec.bottom;

                    overlaywidth = game_rec.right;
                    overlayheight = game_rec.bottom;

                    SetWindowPos(globals.window_handle, 0, xy.x, xy.y, overlaywidth, overlayheight, SWP_NOREDRAW);
                }

                ImGuiIO& io = ImGui::GetIO();

                POINT p_cursor;
                GetCursorPos(&p_cursor);
                io.MousePos.x = p_cursor.x;
                io.MousePos.y = p_cursor.y;

                if (GetAsyncKeyState(VK_LBUTTON)) {
                    io.MouseDown[0] = true;
                    io.MouseClicked[0] = true;
                    io.MouseClickedPos[0].x = io.MousePos.x;
                    io.MouseClickedPos[0].x = io.MousePos.y;
                }
                else
                    io.MouseDown[0] = false;

                draw_instance();

                HWND hwnd = GetWindow(hwnd_active, GW_HWNDPREV);
                if (hwnd_active == globals.window_handle) {
                    hwnd = GetWindow(game_window, GW_HWNDPREV);
                }
                SetWindowPos(globals.window_handle, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }
            else {
                float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                d3d_device_ctx->ClearRenderTargetView(target_view, clearColor);
                d3d_device_ctx->OMSetRenderTargets(1, &target_view, NULL);
                swap_chain->Present(0, 0);
            }
        }

        return 0;
    }

overlay_t overlay;
