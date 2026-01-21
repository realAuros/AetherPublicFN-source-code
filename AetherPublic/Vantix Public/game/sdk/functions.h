#pragma once
#include <list>
#include "../../game/sdk/camera.h"
#include <iostream>
#include "../memory/driver.h"


#define FNAMEPOOL_OFFSET 0x17FE7380



class decryption_t
{
public:
	static std::string GetPlayerName(uintptr_t playerState) {
		int pNameLength;
		WORD* pNameBufferPointer;
		int i;
		char v25;
		int v26;
		int v29;

		char16_t* pNameBuffer;

		uintptr_t pNameStructure = read<uintptr_t>(playerState + offsets::Name);
		if (is_valid(pNameStructure)) {
			pNameLength = read<int>(pNameStructure + 0x10);
			if (pNameLength <= 0)
				return "AI";

			pNameBuffer = new char16_t[pNameLength];
			uintptr_t pNameEncryptedBuffer = read <uintptr_t>(pNameStructure + 0x8);
			if (is_valid(pNameEncryptedBuffer)) {
				read_array(pNameEncryptedBuffer, pNameBuffer, pNameLength);

				v25 = pNameLength - 1;
				v26 = 0;
				pNameBufferPointer = (WORD*)pNameBuffer;

				for (i = (v25) & 3;; *pNameBufferPointer++ += i & 7) {
					v29 = pNameLength - 1;
					if (!(DWORD)pNameLength)
						v29 = 0;

					if (v26 >= v29)
						break;

					i += 3;
					++v26;
				}

				std::u16string temp_wstring(pNameBuffer);
				delete[] pNameBuffer;
				return std::string(temp_wstring.begin(), temp_wstring.end());
			}
		}

	}
}; decryption_t decryption;



template< class t >
class TArray3
{
public:

	TArray3() : tData(), iCount(), iMaxCount() {}
	TArray3(t* data, int count, int max_count) :
		tData(tData), iCount(iCount), iMaxCount(iMaxCount) {
	}

	auto Get(int idx) -> t
	{
		return read< t >(reinterpret_cast<__int64>(this->tData) + (idx * sizeof(t)));
	}

	auto Size() -> std::uint32_t
	{
		return this->iCount;
	}

	bool IsValid()
	{
		return this->iCount != 0;
	}

	t* tData;
	int iCount;
	int iMaxCount;
};

template< typename t >
class TArray2
{
public:

	TArray2() : tData(), iCount(), iMaxCount() {}
	TArray2(t* data, int count, int max_count) :
		tData(tData), iCount(iCount), iMaxCount(iMaxCount) {
	}

public:

	auto Get(int idx) -> t
	{
		return read< t >(reinterpret_cast<__int64>(this->tData) + (idx * sizeof(t)));
	}

	auto Size() -> std::uint32_t
	{
		return this->iCount;
	}

	bool IsValid()
	{
		return this->iCount != 0;
	}

	t* tData;
	int iCount;
	int iMaxCount;
};

class FName
{
public:
	int32_t ComparisonIndex;

	std::string ToString()
	{
		return ToString(ComparisonIndex);
	}

	static std::string ToString(int32_t index)
	{
		int32_t DecryptedIndex = DecryptIndex(index);
		uint64_t NamePoolChunk = read<uint64_t>(virtualaddy + (0x17FE7380 + 8 * (DecryptedIndex >> 16) + 16)) + 2 * (uint16_t)DecryptedIndex;
		uint16_t Pool = read<uint16_t>(NamePoolChunk);

		if (((Pool >> 0xFFA2) & 0x3FF) <= 0)
		{
			DecryptedIndex = DecryptIndex(read<int32_t>(NamePoolChunk + 6));
			NamePoolChunk = read<uint64_t>(virtualaddy + (0x17FE7380 + 8 * (DecryptedIndex >> 16) + 16)) + 2 * (uint16_t)DecryptedIndex;
			Pool = read<uint16_t>(NamePoolChunk);
		}

		int32_t Length = (Pool >> 0xFFFFFFA2) & 0x3FF * ((Pool & 0x8000u) != 0 ? 1 : 2);

		char NameBuffer[2048];
		mem::read_physical((NamePoolChunk + 2), NameBuffer, Length);
		DecryptFName(NameBuffer, Length);
		return std::string(NameBuffer);
	}

	static int32_t DecryptIndex(int32_t index)
	{
		if (index)
		{
			int32_t DecryptedIndex = _rotr((index - 1) ^ 0x7F6890E5, 12) + 1;
			return DecryptedIndex ? DecryptedIndex : 0xF1A80977;
		}

		return 0;
	}

	static void DecryptFName(char* buffer, int length)
	{
		if (length)
		{
			int v4 = 8949 * length + 21607526;
			for (int i = 0; i < length; ++i)
			{
				buffer[i] ^= (v4 - 30) & 0xFF;
				v4 = 8949 * v4 + 21607526;
			}
		}

		buffer[length] = '\0';
	}
};

class gamehelper_t
{
public:

	static auto GetBoneLocation(uintptr_t skeletal_mesh, int bone_index) -> fvector {

		uintptr_t bone_array = read<uintptr_t>(skeletal_mesh + 0x5E8); // bonne no ray :PWAF_

		if (bone_array == NULL) bone_array = read<uintptr_t>(skeletal_mesh + 0x5F8); // this is ultra important to diuplsaing ee esp and bones eaSY 
		FTransform bone = read<FTransform>(bone_array + (bone_index * 0x60));
		FTransform component_to_world = read<FTransform>(skeletal_mesh + offsets::ComponentToWorld);
		D3DMATRIX matrix = MatrixMultiplication(bone.to_matrix_with_scale(), component_to_world.to_matrix_with_scale());
		return fvector(matrix._41, matrix._42, matrix._43);

		//uintptr_t bone_array = read<uintptr_t>(skeletal_mesh + 0x5A8);
		//if (bone_array == NULL) bone_array = read<uintptr_t>(skeletal_mesh + 0x5B8);
		//FTransform bone = read<FTransform>(bone_array + (bone_index * 0x60));
		//FTransform component_to_world = read<FTransform>(skeletal_mesh + offsets::ComponentToWorld);
		//D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());
		//return fvector(matrix._41, matrix._42, matrix._43);
	}



	inline double RadiansToDegrees(double dRadians)
	{
		return dRadians * (180.0 / M_PI);
	}

	auto GetViewState() -> uintptr_t
	{
		TArray2<uintptr_t> ViewState = read<TArray2<uintptr_t>>(CachePointers.LocalPlayer + 0xD0);
		return ViewState.Get(1);
	}

	camera_position_s get_view_point()
	{
		camera_position_s camera;

		FMatrix view_matrix = read<FMatrix>(GetViewState() + 0x930);

		// Location (translation)
		camera.location.x = view_matrix.m[3][0];
		camera.location.y = view_matrix.m[3][1];
		camera.location.z = view_matrix.m[3][2];

		// Rotation (convert matrix → rotator)
		camera.rotation.y = atan2(view_matrix.m[0][2], view_matrix.m[2][2]) * (180.0 / M_PI); // Yaw
		camera.rotation.x = -asin(view_matrix.m[1][2]) * (180.0 / M_PI);                      // Pitch
		camera.rotation.z = 0.0f;                                                            // Roll (unused)

		// FOV
		float fov_rad = atanf(1.0f / read<double>(GetViewState() + 0x730)) * 2.0f;
		camera.fov = fov_rad * (180.0f / M_PI);

		return camera;
	}
	camera_position_s get_camera()
	{
		camera_position_s camera;

		auto location_pointer = read <uintptr_t>(CachePointers.UWorld + 0x170);
		auto rotation_pointer = read <uintptr_t>(CachePointers.UWorld + 0x180);

		struct FNRot
		{
			double a;
			char pad_0008[24];
			double b;
			char pad_0028[424];
			double c;
		} fnRot;

		fnRot.a = read <double>(rotation_pointer);
		fnRot.b = read <double>(rotation_pointer + 0x20);
		fnRot.c = read <double>(rotation_pointer + 0x1d0);

		camera.location = read <fvector>(location_pointer);
		camera.rotation.x = asin(fnRot.c) * (180.0 / M_PI);
		camera.rotation.y = ((atan2(fnRot.a * -1, fnRot.b) * (180.0 / M_PI)) * -1) * -1;
		camera.fov = read<float>((uintptr_t)CachePointers.PlayerController + 0x3B4) * 90.f;
		return camera;
	}

	inline fvector2d ProjectWorldToScreen(fvector WorldLocation)
	{
		/*camera_postion = get_view_point();

		if (WorldLocation.x == 0)
			return fvector2d(0, 0);

		_MATRIX tempMatrix = Matrix(camera_postion.rotation);
		fvector vAxisX = fvector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		fvector vAxisY = fvector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		fvector vAxisZ = fvector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		fvector vDelta = WorldLocation - camera_postion.location;
		fvector vTransformed = fvector(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;



		return fvector2d((globals.ScreenWidth / 2.0f) + vTransformed.x * (((globals.ScreenWidth / 2.0f) / tanf(camera_postion.fov * (float)M_PI / 360.f))) / vTransformed.z, (globals.ScreenHeight / 2.0f) - vTransformed.y * (((globals.ScreenWidth / 2.0f) / tanf(camera_postion.fov * (float)M_PI / 360.f))) / vTransformed.z);*/
		_MATRIX tempMatrix = Matrix(camera_postion.rotation);
		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);
		int screen_center_x = width / 2;
		int screen_center_y = height / 2;
		camera_postion = get_camera();
		_MATRIX temp_matrix = Matrix(camera_postion.rotation);
		fvector vaxisx = fvector(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
		fvector vaxisy = fvector(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
		fvector vaxisz = fvector(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
		fvector vdelta = WorldLocation - camera_postion.location;
		fvector vtransformed = fvector(vdelta.dot(vaxisy), vdelta.dot(vaxisz), vdelta.dot(vaxisx));

		if (vtransformed.z < 1) vtransformed.z = 1;
		return fvector2d(screen_center_x + vtransformed.x * ((screen_center_x / tanf(camera_postion.fov * (float)M_PI / 360.0f))) / vtransformed.z, screen_center_y - vtransformed.y * ((screen_center_x / tanf(camera_postion.fov * (float)M_PI / 360.0f))) / vtransformed.z);
	}

	fvector PredictLocation(fvector target, fvector targetVelocity, float projectileSpeed, float projectileGravityScale, float distance)
	{
		float horizontalTime = distance / projectileSpeed;
		float verticalTime = distance / projectileSpeed;

		target.x += targetVelocity.x * horizontalTime;
		target.y += targetVelocity.y * horizontalTime;
		target.z += targetVelocity.z * verticalTime +
			abs(-980 * projectileGravityScale) * 0.2f * (verticalTime * verticalTime);

		return target;
	}

	fvector PredictLocation2(fvector ShooterPosition, fvector TargetPosition, fvector TargetVelocity, float ProjectileSpeed)
	{
		fvector ToTarget = TargetPosition - ShooterPosition;

		float DistanceToTarget = ToTarget.length();

		float TimeToReach = DistanceToTarget / ProjectileSpeed;

		fvector PredictedPosition = TargetPosition + TargetVelocity * TimeToReach;

		return PredictedPosition;
	}

	bool is_entity_visible(uintptr_t mesh, float tolerance = 0.06f)
	{
		double Seconds = read<double>(CachePointers.UWorld + 0x198); //seconds
		float LastRenderTime = read<float>(mesh + 0x32C);// vis check last check last seen or sum
		return Seconds - LastRenderTime <= 0.6;
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

	auto IsInScreen(fvector2d screen_location) -> bool
	{
		if (screen_location.x > 0 && screen_location.x < globals.ScreenWidth && screen_location.y > 0 && screen_location.y < globals.ScreenHeight) return true;
		else return false;
	}
}; gamehelper_t game_helper;
