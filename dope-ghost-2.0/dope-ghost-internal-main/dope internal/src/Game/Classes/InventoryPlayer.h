#pragma once

class InventoryPlayer
{
public:
	int GetSlot(JNIEnv* env);
	void SetSlot(int slot, JNIEnv* env);
	jobject GetStackInSlot(int slot, JNIEnv* env);
};