#include "pch.h"
#include "GuiScreen.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

bool GuiScreen::IsInventory(JNIEnv* env)
{
    if (this == NULL)
        return NULL;

    const auto klass = g_Instance->FindClass(Mapper::Get("net/minecraft/client/gui/inventory/GuiInventory"));
    if (!klass)
        return NULL;

    return env->IsInstanceOf((jobject)this, (jclass)klass);
}

bool GuiScreen::IsChat(JNIEnv* env)
{
    if (this == NULL)
        return NULL;

    const auto klass = g_Instance->FindClass(Mapper::Get("net/minecraft/client/gui/GuiChat"));
	if (!klass)
		return NULL;

    return env->IsInstanceOf((jobject)this, (jclass)klass);
}