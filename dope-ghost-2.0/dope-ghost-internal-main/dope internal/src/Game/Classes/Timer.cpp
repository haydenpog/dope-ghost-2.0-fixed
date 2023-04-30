#include "pch.h"
#include "Timer.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

float Timer::GetRenderPartialTicks(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto timerClazz = (Klass*)env->GetObjectClass((jobject)this);
	const auto yawOffsetField = timerClazz->GetField(env, Mapper::Get("renderPartialTicks").data(), "F");

	if (timerClazz)
		env->DeleteLocalRef((jclass)timerClazz);

	return yawOffsetField->GetFloatField(env, this);
}

float Timer::GetTimerSpeed(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto timerClazz = (Klass*)env->GetObjectClass((jobject)this);
	const auto timerSpeed = timerClazz->GetField(env, Mapper::Get("timerSpeed").data(), "F");

	if (timerClazz)
		env->DeleteLocalRef((jclass)timerClazz);

	return timerSpeed->GetFloatField(env, this);
}

void Timer::SetTimerSpeed(float v, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto timerClazz = (Klass*)env->GetObjectClass((jobject)this);
	const auto timerSpeed = timerClazz->GetField(env, Mapper::Get("timerSpeed").data(), "F");

	if (timerClazz)
		env->DeleteLocalRef((jclass)timerClazz);

	timerSpeed->SetFloatField(env, this, v);
}