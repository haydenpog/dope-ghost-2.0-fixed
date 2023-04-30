#include "pch.h"
#include "Gui.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

void Gui::DrawRect(int left, int right, int bottom, int top, int color, JNIEnv* env)
{
	Klass* guiClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/gui/Gui"));
	Method* drawRectMethod = guiClazz->GetMethod(env, Mapper::Get("drawRect").data(), "(IIIII)V", true);
	drawRectMethod->CallVoidMethod(env, guiClazz, true, left, top, right, bottom, color);
}
