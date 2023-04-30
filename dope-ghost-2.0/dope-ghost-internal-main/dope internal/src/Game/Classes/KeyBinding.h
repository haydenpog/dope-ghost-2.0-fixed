#pragma once

class KeyBinding
{
public:
	void SetPressed(bool v, JNIEnv* env);
	bool IsPressed(JNIEnv* env);
};