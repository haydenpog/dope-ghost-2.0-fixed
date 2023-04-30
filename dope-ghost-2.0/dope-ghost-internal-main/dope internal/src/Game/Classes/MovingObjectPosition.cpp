#include "pch.h"
#include "MovingObjectPosition.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

bool MovingObjectPosition::IsAimingBlock(JNIEnv* env)
{
	if (this == NULL || env == NULL)
		return false;

	const auto movingObjectClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/util/MovingObjectPosition"));
	const auto typeOfHitField = movingObjectClazz->GetField(env, Mapper::Get("typeOfHit").data(), Mapper::Get("net/minecraft/util/MovingObjectPosition$MovingObjectType", 2).data());
	const auto typeOfHitObject = typeOfHitField->GetObjectField(env, (jobject)this);

	const auto movingObjectTypeClazz = (Klass*)env->GetObjectClass((jobject)typeOfHitObject);
	const auto enumBLOCKField = movingObjectTypeClazz->GetField(env, Mapper::Get("BLOCK").data(), Mapper::Get("net/minecraft/util/MovingObjectPosition$MovingObjectType", 2).data(), true);

	const auto obj = enumBLOCKField->GetObjectField(env, movingObjectTypeClazz, true);
	if (!obj) {
		return false;
	}

	if (!typeOfHitObject) {
		return false;
	}

	return env->IsSameObject(obj, typeOfHitObject);
}

bool MovingObjectPosition::IsAimingEntity(JNIEnv* env)
{
	if (this == NULL || env == NULL)
		return false;

	const auto movingObjectClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/util/MovingObjectPosition"));
	const auto typeOfHitField = movingObjectClazz->GetField(env, Mapper::Get("typeOfHit").data(), Mapper::Get("net/minecraft/util/MovingObjectPosition$MovingObjectType", 2).data());
	const auto typeOfHitObject = typeOfHitField->GetObjectField(env, (jobject)this);

	const auto movingObjectTypeClazz = (Klass*)env->GetObjectClass((jobject)typeOfHitObject);
	const auto enumBLOCKField = movingObjectTypeClazz->GetField(env, Mapper::Get("ENTITY").data(), Mapper::Get("net/minecraft/util/MovingObjectPosition$MovingObjectType", 2).data(), true);
	const auto obj = enumBLOCKField->GetObjectField(env, movingObjectTypeClazz, true);
	if (!obj || !typeOfHitObject)
		return false;

	return env->IsSameObject(obj, typeOfHitObject);
}
