
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include "game/memory/driver.h"
#include "game/memory/mapper/mapper.hpp"
#include "backend/imgui_impl_dx11.h"
#include "backend/imgui_impl_win32.h"
#include "crypt.hxx"
#include "game/render.hxx"
#include "auth.h"
#include "game/settings.hxx"

using namespace KeyAuth;
api KeyAuthApp(
	hash_string("").decrypt(),
	hash_string("").decrypt(),
	hash_string("1.0").decrypt(),
	hash_string("https://keyauth.win/api/1.3/").decrypt(),
	hash_string("").decrypt()
);
void display_with_delay(const std::string& text, int delayMs) {
	for (const char& c : text) {
		std::cout << c;
		std::cout.flush();
		Sleep(delayMs);
	}
}

void SuppressConsoleOutput() {
	std::freopen("NUL", "w", stdout);
}

HHOOK hHook;
HWND hwnd;


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		if (wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT* pkbs = (KBDLLHOOKSTRUCT*)lParam;
			if (pkbs->vkCode == VK_INSERT) {
				if (hwnd) {
					DestroyWindow(hwnd);
				}
				return 1;
			}
		}
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}


void SetHook() {
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
}


void RemoveHook() {
	UnhookWindowsHookEx(hHook);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);


		int width = rect.right - rect.left;
		int newWidth = static_cast<int>(width * 0.8); // 80% <->


		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));


		HFONT hFont = CreateFont(
			100, // wysokość czcionki
			0, // szerokość czcionki (0 oznacza automatyczne dostosowanie)
			0, // kąt obrotu
			0, // kąt obrotu
			FW_BOLD, // grubość czcionki
			FALSE, // kursywa
			FALSE, // podkreślenie
			FALSE, // przekreślenie
			DEFAULT_CHARSET, // zestaw znaków
			OUT_OUTLINE_PRECIS, // precyzja wyjścia
			CLIP_DEFAULT_PRECIS, // precyzja przycinania
			CLEARTYPE_QUALITY, // jakość
			DEFAULT_QUALITY, // ogólna jakość
			TEXT("Arial") // czcionka
		);
		SelectObject(hdc, hFont);


		std::string message = hash_string("Initialized, Press Insert in Lobby!").decrypt();
		RECT textRect;


		SetRect(&textRect, (width - newWidth) / 2, rect.top, (width + newWidth) / 2, rect.bottom);


		DrawTextA(hdc, message.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORDBREAK);

		DeleteObject(hFont);
		EndPaint(hwnd, &ps);
	}
				 break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


std::string key;
std::string user;

bool auth_success;
#include <filesystem>

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)hash_string(__DATE__);
const std::string compilation_time = (std::string)hash_string(__TIME__);
void sessionStatus();
void ShowMessage() {
	const char CLASS_NAME[] = "Sample Window Class";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST,
		CLASS_NAME,
		hash_string("Message"),
		WS_POPUP | WS_VISIBLE,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		nullptr, nullptr, GetModuleHandle(nullptr), nullptr
	);

	if (hwnd) {
		SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY); //  przezroczystosc
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
	}
}

int main() {

	/*if (a != 0)*/

		//GlobalDeleteAtom((a));

	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	std::string exeName = std::filesystem::path(path).filename().string();
	if (exeName != hash_string("AetherPublic.exe").decrypt()) {
		MessageBoxA(NULL, hash_string("Do not change the name of the executable file!").decrypt(), hash_string("Protection").decrypt(), MB_OK | MB_ICONERROR);
		return 0;

	}
	KeyAuthApp.init();
	Beep(150, 300);

	//VMProtectBeginUltra("1");
	//KeyAuthApp.init();
	Beep(150, 400);
	Beep(150, 400);
	Sleep(800);

	std::string key;
	std::string username;


	std::string consoleTitle = hash_string("Loader: 1.0").decrypt();
	SetConsoleTitleA(consoleTitle.c_str());

	system(hash_string("cls"));

	bool hasKey = false;
	bool hasUsername = false;


	std::ifstream inFile(hash_string("PRIVATE.auth.api").decrypt());
	if (inFile.is_open() && std::filesystem::file_size(hash_string("PRIVATE.auth.api").decrypt()) > 0) {
		std::getline(inFile, key);
		if (!key.empty()) {
			hasKey = true;
			std::cout << hash_string("Found saved License Key -> ").decrypt() << key << "\n";
			KeyAuthApp.license(key);
			if (!KeyAuthApp.response.success) {
				exit(1336);
			}
			else
			{

				auth_success = true;
			}
		}

		std::getline(inFile, username);
		if (!username.empty()) {
			hasUsername = true;
			std::cout << hash_string("Found saved Discord Username -> ").decrypt() << username << "\n";
		}

		inFile.close();
		Sleep(1000);
	}


	if (!hasKey || !hasUsername) {

		if (!hasKey) {
			display_with_delay(hash_string("Enter License Key -> ").decrypt(), 20);
			std::cin >> key;
			KeyAuthApp.license(key);
			if (!KeyAuthApp.response.success) {
				exit(1336);
			}
			else
			{

				auth_success = true;
			}
			// Checking the license code. Enter the ApplicationID here. (required, do not delete this code)
		}


		if (!hasUsername) {
			display_with_delay(hash_string("Enter Discord Username -> ").decrypt(), 20);
			std::cin >> username;
		}


		std::string decision;
		display_with_delay(hash_string("Do You want to save License? (y/n): ").decrypt(), 20);
		std::cin >> decision;

		if (decision == hash_string("y").decrypt() || decision == hash_string("Y").decrypt()) {
			// saving nigga
			std::ofstream outFile(hash_string("PRIVATE.auth.api").decrypt(), std::ios::out);
			if (outFile.is_open()) {
				outFile << key << "\n" << username;
				outFile.close();
				// saved
			}
			else {
				// failed to save // not saved
			}
		}
		else {
			// not saved 
		}
	}





	SuppressConsoleOutput();
	std::freopen("CON", "w", stdout);

	Sleep(3000);

	system(hash_string("cls"));

	display_with_delay("\n\n", 20);





	int result = MessageBoxA(
		NULL,                    // No parent window
		hash_string("Do you want to Load Driver\n    LOAD ONCE PER BOOT!").decrypt(), // Message
		hash_string("Aether | Protection!").decrypt(),          // Title
		MB_YESNO | MB_ICONQUESTION // Buttons and icon
	);

	if (result == IDYES) {
		Beep(800, 250);
		Beep(1000, 400);
		Beep(1200, 550);
		Hasty::Load_Dependencies();
	}
	if (mem::init())
	{
		Beep(1200, 450);
	}




	globals.ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	globals.ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	Beep(500, 500);
	Beep(500, 500);
	SetHook();
	ShowMessage();
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	RemoveHook();
	Beep(500, 400);
	Sleep(200);
	display_with_delay(hash_string("Looking For Game Process").decrypt(), 20);
	if (!mem::get_pid(hash_string("FortniteClient-Win64-Shipping.exe").decrypt())) { std::cout << hash_string("-> The game process wasn't found").decrypt() << std::endl; }
	if (virtualaddy = mem::base_address()) { std::cout << hash_string("Base Address sucessfully retrieved").decrypt() << std::endl; }

	Sleep(1500);


	mem::fetch_cr3();

	overlay.initialize_overlay();
	overlay.initialize_imgui();

	//std::thread(niga).detach();
	overlay.thread();
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)overlay.render_loop(), NULL, 0, NULL);






	return 0;


}

