#include "pch.h"
#include "GameSettings.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

jobject GameSettings::GetKeyBindSneak(JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto gameSettingsClass = (Klass*)env->GetObjectClass((jobject)this);
	const auto keyBindSneakField = gameSettingsClass->GetField(env, Mapper::Get("keyBindSneak").data(), Mapper::Get("net/minecraft/client/settings/KeyBinding", 2).data());

	if (gameSettingsClass)
		env->DeleteLocalRef((jclass)gameSettingsClass);

	return keyBindSneakField->GetObjectField(env, (jobject)this);
}
