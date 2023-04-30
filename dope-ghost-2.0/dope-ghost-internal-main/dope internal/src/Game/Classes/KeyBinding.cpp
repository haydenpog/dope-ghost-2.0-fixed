#include "pch.h"
#include "KeyBinding.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

void KeyBinding::SetPressed(bool v, JNIEnv* env)
{
	if (this == NULL)
		return;

	const auto keybindingClass = (Klass*)env->GetObjectClass((jobject)this);
	const auto pressedField = keybindingClass->GetField(env, Mapper::Get("pressed").data(), "Z");

	if (keybindingClass)
		env->DeleteLocalRef((jclass)keybindingClass);

	pressedField->SetBooleanField(env, this, v);
}

bool KeyBinding::IsPressed(JNIEnv* env)
{
	if (this == NULL)
		return false;

	const auto keybindingClass = (Klass*)env->GetObjectClass((jobject)this);
	const auto pressedField = keybindingClass->GetField(env, Mapper::Get("pressed").data(), "Z");

	if (keybindingClass)
		env->DeleteLocalRef((jclass)keybindingClass);

	return pressedField->GetBooleanField(env, this);
}
