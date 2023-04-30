#include "pch.h"
#include "Block.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

int Block::GetID(JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	static Method* getIDMethod = 0;
	const auto blockClass = (Klass*)env->GetObjectClass((jobject)this);

	if (!getIDMethod) {
		char* type = new char[256];
		sprintf(type, "(%s)I", Mapper::Get("net/minecraft/block/Block", 2).data());
		getIDMethod = blockClass->GetMethod(env, Mapper::Get("getIdFromBlock").data(), type, true);
		delete[] type;
	}

	const auto ret = getIDMethod->CallIntMethod(env, blockClass, true, (jobject)this);

	if (blockClass)
		env->DeleteLocalRef((jclass)blockClass);
	return ret;
}

bool Block::IsAir(JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto klass = g_Instance->FindClass(Mapper::Get("net/minecraft/block/BlockAir"));
	if (!klass)
		return NULL;

	return env->IsInstanceOf((jobject)this, (jclass)klass);
}
