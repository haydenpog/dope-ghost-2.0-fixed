#pragma once

class ItemStack
{
public:
	jobject GetItem(JNIEnv* env);
	bool IsWeapon(JNIEnv* env);
	bool IsBlock(JNIEnv* env);
	bool IsEnderPearl(JNIEnv* env);
	bool Is(const char* clazz, JNIEnv* env);
	int GetMetadata(JNIEnv* env);
};