#include "pch.h"
#include "Minecraft.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"
#include "../../Cheat/Modules/Settings.h"

jobject Minecraft::GetTheMinecraft(JNIEnv* env)
{
	const auto clazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto theMinecraftField = clazz->GetField(env, Mapper::Get("theMinecraft").data(), Mapper::Get("net/minecraft/client/Minecraft", 2).data(), true);
	return theMinecraftField->GetObjectField(env, clazz, true);
}

jobject Minecraft::GetThePlayer(JNIEnv* env)
{
	const auto clazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto thePlayerField = clazz->GetField(env, Mapper::Get("thePlayer").data(), Mapper::Get("net/minecraft/client/entity/EntityClientPlayerMP", 2).data(), false);

	return thePlayerField->GetObjectField(env, GetTheMinecraft(env), false);
}

jobject Minecraft::GetTheWorld(JNIEnv* env)
{
	const auto clazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto thePlayerField = clazz->GetField(env, Mapper::Get("theWorld").data(), Mapper::Get("net/minecraft/client/multiplayer/WorldClient", 2).data(), false);

	return thePlayerField->GetObjectField(env, GetTheMinecraft(env), false);
}

jobject Minecraft::GetCurrentScreen(JNIEnv* env)
{
	const auto clazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto thePlayerField = clazz->GetField(env, Mapper::Get("currentScreen").data(), Mapper::Get("net/minecraft/client/gui/GuiScreen", 2).data(), false);

	return thePlayerField->GetObjectField(env, GetTheMinecraft(env), false);
}

jobject Minecraft::GetRenderManager(JNIEnv* env)
{
	const auto minecraftClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto renderManagerClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/renderer/entity/RenderManager"));

	if (g_GameVersion == CASUAL_1_7_10 || g_GameVersion == FORGE_1_7_10 || g_GameVersion == LUNAR_1_7_10) {
		const auto renderManagerInstanceField = renderManagerClazz->GetField(env, Mapper::Get("renderManagerInstance").data(), Mapper::Get("net/minecraft/client/renderer/entity/RenderManager", 2).data(), true);
		return renderManagerInstanceField->GetObjectField(env, renderManagerClazz, true);
	}
	else {
		const auto renderManagerInstanceMethod = minecraftClazz->GetMethod(env, Mapper::Get("getRenderManager").data(), Mapper::Get("net/minecraft/client/renderer/entity/RenderManager", 3).data());
		return renderManagerInstanceMethod->CallObjectMethod(env, GetTheMinecraft(env));
	}
}

jobject Minecraft::GetTimer(JNIEnv* env)
{
	const auto minecraftClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto timerField = minecraftClazz->GetField(env, Mapper::Get("timer").data(), Mapper::Get("net/minecraft/util/Timer", 2).data());

	return timerField->GetObjectField(env, GetTheMinecraft(env));
}

jobject Minecraft::GetFontRenderer(JNIEnv* env)
{
	const auto minecraftClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto fontRendererField = minecraftClazz->GetField(env, Mapper::Get("fontRendererObj").data(), Mapper::Get("net/minecraft/client/gui/FontRenderer", 2).data());

	return fontRendererField->GetObjectField(env, GetTheMinecraft(env));
}

jobject Minecraft::GetObjectMouseOver(JNIEnv* env)
{
	const auto minecraftClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto objectMouseOverField = minecraftClazz->GetField(env, Mapper::Get("objectMouseOver").data(), Mapper::Get("net/minecraft/util/MovingObjectPosition", 2).data());

	return objectMouseOverField->GetObjectField(env, GetTheMinecraft(env));
}

jobject Minecraft::GetGameSettings(JNIEnv* env)
{
	const auto minecraftClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto gameSettingsField = minecraftClazz->GetField(env, Mapper::Get("gameSettings").data(), Mapper::Get("net/minecraft/client/settings/GameSettings", 2).data());

	return gameSettingsField->GetObjectField(env, GetTheMinecraft(env));
}

int Minecraft::GetDisplayHeight(JNIEnv* env)
{
	const auto minecraftClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto displayHeightField = minecraftClazz->GetField(env, Mapper::Get("displayHeight").data(), "I");

	return displayHeightField->GetIntField(env, GetTheMinecraft(env));
}

int Minecraft::GetDisplayWidth(JNIEnv* env)
{
	const auto minecraftClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/Minecraft"));
	const auto displayHeightField = minecraftClazz->GetField(env, Mapper::Get("displayWidth").data(), "I");

	return displayHeightField->GetIntField(env, GetTheMinecraft(env));
}
