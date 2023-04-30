#include "pch.h"
#include "ItemStack.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

jobject ItemStack::GetItem(JNIEnv* env)
{
	if (this == NULL)
		return NULL;
	
	const auto itemStackClass = g_Instance->FindClass(Mapper::Get("net/minecraft/item/ItemStack"));
	const auto getItemMethod = itemStackClass->GetMethod(env, Mapper::Get("getItem").data(), Mapper::Get("net/minecraft/item/Item", 3).data());

	return getItemMethod->CallObjectMethod(env, (jobject)this);
}

bool ItemStack::IsWeapon(JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto heldItem = this->GetItem(env);

	const auto axeKlass = g_Instance->FindClass(Mapper::Get("net/minecraft/item/ItemAxe"));
	const auto swordKlass = g_Instance->FindClass(Mapper::Get("net/minecraft/item/ItemSword"));
	if (!axeKlass || !swordKlass) {
		return NULL;
	}

	return env->IsInstanceOf(heldItem, (jclass)axeKlass)
		|| env->IsInstanceOf(heldItem, (jclass)swordKlass);
}

bool ItemStack::IsBlock(JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto heldItem = this->GetItem(env);
	const auto klass = g_Instance->FindClass(Mapper::Get("net/minecraft/item/ItemBlock"));
	if (!klass)
		return NULL;

	return env->IsInstanceOf(heldItem, (jclass)klass);
}

bool ItemStack::IsEnderPearl(JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto heldItem = this->GetItem(env);
	const auto klass = g_Instance->FindClass(Mapper::Get("net/minecraft/item/ItemEnderPearl"));
	if (!klass)
		return NULL;

	return env->IsInstanceOf(heldItem, (jclass)klass);
}

bool ItemStack::Is(const char* clazz, JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto heldItem = this->GetItem(env);
	const auto klass = g_Instance->FindClass(clazz);
	if (!klass)
		return NULL;

	return env->IsInstanceOf(heldItem, (jclass)klass);
}

int ItemStack::GetMetadata(JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto itemStackClass = g_Instance->FindClass(Mapper::Get("net/minecraft/item/ItemStack"));
	const auto metadataField = itemStackClass->GetField(env, Mapper::Get("metadata").data(), "I");
	return metadataField->GetIntField(env, (jobject)this);
}
