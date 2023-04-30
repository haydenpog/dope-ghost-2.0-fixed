#include "pch.h"
#include "InventoryPlayer.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

int InventoryPlayer::GetSlot(JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto inventoryPlayerClass = (Klass*)env->GetObjectClass((jobject)this);
	const auto hotbarSlotField = inventoryPlayerClass->GetField(env, Mapper::Get("currentItem").data(), "I");

	if (inventoryPlayerClass)
		env->DeleteLocalRef((jclass)inventoryPlayerClass);

	return hotbarSlotField->GetIntField(env, this);
}

void InventoryPlayer::SetSlot(int slot, JNIEnv* env)
{
	if (this == NULL)
		return;

	const auto inventoryPlayerClass = (Klass*)env->GetObjectClass((jobject)this);
	const auto hotbarSlotField = inventoryPlayerClass->GetField(env, Mapper::Get("currentItem").data(), "I");

	if (inventoryPlayerClass)
		env->DeleteLocalRef((jclass)inventoryPlayerClass);

	hotbarSlotField->SetIntField(env, this, slot);
}

jobject InventoryPlayer::GetStackInSlot(int slot, JNIEnv* env)
{
	if (this == NULL)
		return NULL;

	const auto inventoryPlayerClass = (Klass*)env->GetObjectClass((jobject)this);

	char* methodClass = new char[126];
	sprintf(methodClass, "(I)%s", Mapper::Get("net/minecraft/item/ItemStack", 2).data());
	const auto getStackInSlotMethod = inventoryPlayerClass->GetMethod(env, Mapper::Get("getStackInSlot").data(), methodClass);
	delete[] methodClass;

	if (inventoryPlayerClass)
		env->DeleteLocalRef((jclass)inventoryPlayerClass);

	return getStackInSlotMethod->CallObjectMethod(env, (jobject)this, false, slot);
}
