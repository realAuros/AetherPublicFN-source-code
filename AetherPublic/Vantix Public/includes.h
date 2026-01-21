#pragma once
#include "game/sdk/sdk.h"
#include <windows.h>
#include "backend/imgui_impl_win32.h"
#include "game/memory/driver.h"
#include <dwmapi.h>
#include "crypt.hxx"
#include "game/offsets.h"
#include "game/settings.hxx"

ImFont* MenuFont;
ImFont* GameFont;

class structs
{
public:

	uintptr_t
		UWorld,
		GameInstance,
		GameState,
		LocalPlayer,
		AcknownledgedPawn,
		PlayerState,
		PlayerController,
		RootComponent,
		Mesh,
		PlayerArray,
		LocalWeapon;
	int32_t
		AmmoCount;

	fvector
		RelativeLocation;

	int
		PlayerArraySize,
		TeamIndex;

}; structs CachePointers;

class entity {
public:
	uintptr_t
		entity,
		skeletal_mesh,
		root_component,
		player_state;

	char
		IgnoreDeads;

	int
		team_index,
		kills;
	char
		team_number;
	float
		lastrendertime;
	bool
		is_visible;
};
std::vector<entity> entity_list;
std::vector<entity> temporary_entity_list;

class item {
public:
	uintptr_t
		Actor;

	std::string
		Name;
	bool
		isVehicle,
		isChest,
		isPickup,
		isAmmoBox;
	float
		distance;
};

std::vector<item> item_pawns;