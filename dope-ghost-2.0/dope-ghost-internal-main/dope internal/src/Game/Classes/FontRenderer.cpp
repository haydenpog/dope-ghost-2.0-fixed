#include "pch.h"
#include "FontRenderer.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"
#include "../../Cheat/Modules/Settings.h"

int FontRenderer::GetStringWidth(const char* str, JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto fontRendererClazz = (Klass*)env->GetObjectClass((jobject)this);
	const auto getStringWidthMethod = fontRendererClazz->GetMethod(env, Mapper::Get("getStringWidth").data(), "(Ljava/lang/String;)I");

	const jstring jstringText = env->NewStringUTF(str);
	const auto ret = getStringWidthMethod->CallIntMethod(env, (jobject)this, false, jstringText);

	if (fontRendererClazz)
		env->DeleteLocalRef((jclass)fontRendererClazz);

	if (jstringText)
		env->DeleteLocalRef(jstringText);

	return ret;
}

void FontRenderer::DrawString(const char* str, float x, float y, char dropShadow, int color, JNIEnv* env)
{
	if (this == NULL)
		return;

	const auto fontRendererClazz = (Klass*)env->GetObjectClass((jobject)this);
	const jstring jstringText = env->NewStringUTF(str);
	
	if (g_GameVersion == CASUAL_1_7_10 || g_GameVersion == FORGE_1_7_10 || g_GameVersion == LUNAR_1_7_10) {
		const auto drawStringMethod = fontRendererClazz->GetMethod(env, Mapper::Get("drawString").data(), "(Ljava/lang/String;IIIZ)I");
		drawStringMethod->CallVoidMethod(env, (jobject)this, false, jstringText, int(x), int(y), color, dropShadow);
	}
	else {
		const auto drawStringMethod = fontRendererClazz->GetMethod(env, Mapper::Get("drawString").data(), "(Ljava/lang/String;FFIZ)I");
		drawStringMethod->CallVoidMethod(env, (jobject)this, false, jstringText, x, y, color, dropShadow);
	}

	if (fontRendererClazz)
		env->DeleteLocalRef((jclass)fontRendererClazz);

	if (jstringText)
		env->DeleteLocalRef(jstringText);
}
