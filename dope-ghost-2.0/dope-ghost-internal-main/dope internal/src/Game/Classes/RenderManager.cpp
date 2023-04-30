#include "pch.h"
#include "RenderManager.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"
#include "../../Cheat/Modules/Settings.h"

jobject RenderManager::getEntityRender(jobject argEntity, JNIEnv* env)
{
	if (this == NULL || !env)
		return NULL;

	const auto renderManagerClazz = (Klass*)env->GetObjectClass((jobject)this);

	const auto getEntityRenderMethod = renderManagerClazz->GetMethod(env, Mapper::Get("getEntityRenderObject").data(),
		std::string("(" + Mapper::Get("net/minecraft/entity/Entity", 2) + ")" + Mapper::Get("net/minecraft/client/renderer/entity/Render", 2)).data());

	if (renderManagerClazz)
		env->DeleteLocalRef((jclass)renderManagerClazz);

	return getEntityRenderMethod->CallObjectMethod(env, this, false, argEntity);
}

Vec3D RenderManager::GetRenderPos(JNIEnv* env)
{
	if (this == NULL || !env)
		return Vec3D();

	const auto renderManagerClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/renderer/entity/RenderManager"));
	if (g_GameVersion == CASUAL_1_7_10 || g_GameVersion == FORGE_1_7_10 || g_GameVersion == LUNAR_1_7_10) {
		const auto renderPosFieldX = renderManagerClazz->GetField(env, Mapper::Get("renderPosX").data(), "D", true);
		const auto renderPosFieldY = renderManagerClazz->GetField(env, Mapper::Get("renderPosY").data(), "D", true);
		const auto renderPosFieldZ = renderManagerClazz->GetField(env, Mapper::Get("renderPosZ").data(), "D", true);
		return Vec3D{ renderPosFieldX->GetDoubleField(env, (jobject)this, true), 
			renderPosFieldY->GetDoubleField(env, (jobject)this, true),
			renderPosFieldZ->GetDoubleField(env, (jobject)this, true)
		};
	}
	else {
		const auto renderPosFieldX = renderManagerClazz->GetField(env, Mapper::Get("renderPosX").data(), "D");
		const auto renderPosFieldY = renderManagerClazz->GetField(env, Mapper::Get("renderPosY").data(), "D");
		const auto renderPosFieldZ = renderManagerClazz->GetField(env, Mapper::Get("renderPosZ").data(), "D");

		return Vec3D{ renderPosFieldX->GetDoubleField(env, (jobject)this), 
			renderPosFieldY->GetDoubleField(env, (jobject)this), 
			renderPosFieldZ->GetDoubleField(env, (jobject)this) 
		};
	}

	return Vec3D();
}

Vec3D RenderManager::GetViewerPos(JNIEnv* env)
{
	if (this == NULL || !env)
		return Vec3D();

	const auto renderManagerClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/renderer/entity/RenderManager"));
	const auto fieldX = renderManagerClazz->GetField(env, Mapper::Get("viewerPosX").data(), "D");
	const auto fieldY = renderManagerClazz->GetField(env, Mapper::Get("viewerPosY").data(), "D");
	const auto fieldZ = renderManagerClazz->GetField(env, Mapper::Get("viewerPosZ").data(), "D");

	return Vec3D(fieldX->GetDoubleField(env, (jobject)this),
		fieldY->GetDoubleField(env, (jobject)this),
		fieldZ->GetDoubleField(env, (jobject)this)
	);
}

bool RenderManager::DoRenderEntity(jobject entity, double x, double y, double z, float entityYaw, float partialTicks, bool p_147939_10_, JNIEnv* env) // weird af
{
	if (this == NULL || !env)
		return false;

	const auto renderManagerClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/client/renderer/entity/RenderManager"));
	const auto doRenderEntityMethod = renderManagerClazz->GetMethod(env, "doRenderEntity", "(Lnet/minecraft/entity/Entity;DDDFFZ)Z");
	return doRenderEntityMethod->CallBoolMethod(env, (jobject)this, false, entity, x, y, z, entityYaw, partialTicks, p_147939_10_);
}
