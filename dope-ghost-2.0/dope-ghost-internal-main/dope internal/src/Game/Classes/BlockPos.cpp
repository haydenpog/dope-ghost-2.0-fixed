#include "pch.h"
#include "BlockPos.h"
#include "Minecraft.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"
#include "../../Cheat/Modules/Settings.h"

jobject BlockPos::GetBlock(Vec3D pos, jobject worldObject, JNIEnv* env)
{
	if (g_GameVersion == CASUAL_1_7_10 || g_GameVersion == FORGE_1_7_10 || g_GameVersion == LUNAR_1_7_10) {
		char* type = new char[256];
		sprintf(type, "(III)%s", Mapper::Get("net/minecraft/block/Block", 2).data());

		const auto worldClazz = (Klass*)env->GetObjectClass(worldObject);
		const auto getBlockMethod = worldClazz->GetMethod(env, Mapper::Get("getBlock").data(), type);
		delete[] type;

		if (worldClazz)
			env->DeleteLocalRef((jclass)worldClazz);
		return getBlockMethod->CallObjectMethod(env, (jobject)worldObject, false, (int)pos.x, (int)pos.y, (int)pos.z);
	}
	else {
		static auto blockPosClass = g_Instance->FindClass(Mapper::Get("net/minecraft/util/BlockPos"));
		static auto iBlockStateClass = g_Instance->FindClass(Mapper::Get("net/minecraft/block/state/IBlockState"));
		const auto worldClazz = (Klass*)env->GetObjectClass(worldObject);
		static auto constructor = blockPosClass->GetMethod(env, "<init>", "(DDD)V");
		const auto blockPosObj = env->NewObject((jclass)blockPosClass, (jmethodID)constructor, pos.x, pos.y, pos.z);
		static auto getBlockMethod = iBlockStateClass->GetMethod(env, Mapper::Get("getBlock").data(), Mapper::Get("net/minecraft/block/Block", 3).data());

		char* type = new char[256];
		sprintf(type, "(%s)%s", Mapper::Get("net/minecraft/util/BlockPos", 2).data(), Mapper::Get("net/minecraft/block/state/IBlockState", 2).data());
		const auto getBlockStateMethod = worldClazz->GetMethod(env, Mapper::Get("getBlockState").data(), type);
		delete[] type;

		const auto blockState = getBlockStateMethod->CallObjectMethod(env, worldObject, false, (jobject)blockPosObj);

		if (worldClazz)
			env->DeleteLocalRef((jclass)worldClazz);

		if (blockPosObj)
			env->DeleteLocalRef(blockPosObj);

		return getBlockMethod->CallObjectMethod(env, blockState);
	}
}
