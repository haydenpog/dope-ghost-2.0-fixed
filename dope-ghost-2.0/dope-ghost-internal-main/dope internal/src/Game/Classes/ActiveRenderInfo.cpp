#include "pch.h"
#include "ActiveRenderInfo.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

std::vector<float> ActiveRenderInfo::GetProjection(JNIEnv* env)
{
	const auto activeRenderInfoClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/renderer/ActiveRenderInfo"));
	const auto projectionField = activeRenderInfoClazz->GetField(env, Mapper::Get("PROJECTION").data(), "Ljava/nio/FloatBuffer;", true);
	const auto projectionObject = projectionField->GetObjectField(env, activeRenderInfoClazz, true);

	const auto floatBufferClazz = g_Instance->FindClass("java/nio/FloatBuffer");
	const auto getMethod = floatBufferClazz->GetMethod(env, "get", "(I)F");

	std::vector<float> ret;
	for (int i = 0; i < 16; i++)
	{
		ret.push_back(getMethod->CallFloatMethod(env, projectionObject, false, i));
	}

	if (projectionObject)
		env->DeleteLocalRef((jobject)projectionObject);
	return ret;
}

std::vector<float> ActiveRenderInfo::GetModelView(JNIEnv* env)
{
	const auto activeRenderInfoClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/renderer/ActiveRenderInfo"));

	const auto projectionField = activeRenderInfoClazz->GetField(env, Mapper::Get("MODELVIEW").data(), "Ljava/nio/FloatBuffer;", true);
	const auto projectionObject = projectionField->GetObjectField(env, activeRenderInfoClazz, true);

	const auto floatBufferClazz = g_Instance->FindClass("java/nio/FloatBuffer");
	const auto getMethod = floatBufferClazz->GetMethod(env, "get", "(I)F");

	std::vector<float> ret;
	for (int i = 0; i < 16; i++)
	{
		ret.push_back(getMethod->CallFloatMethod(env, projectionObject, false, i));
	}

	if (projectionObject)
		env->DeleteLocalRef((jobject)projectionObject);
	return ret;
}