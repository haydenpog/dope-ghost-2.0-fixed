#pragma once

class Field;
class Method;
class Klass
{
public:
	const char* GetName(JNIEnv* env);
	Field* GetField(JNIEnv* env, const char* name, const char* sig, bool staticField = false);
	Method* GetMethod(JNIEnv* env, const char* name, const char* sig, bool staticMethod = false);
};