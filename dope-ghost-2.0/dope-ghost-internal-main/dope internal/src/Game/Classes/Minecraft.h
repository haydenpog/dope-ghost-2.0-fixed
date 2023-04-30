#pragma once

class Minecraft
{
public:
	static jobject GetTheMinecraft(JNIEnv* env);
	static jobject GetThePlayer(JNIEnv* env);
	static jobject GetTheWorld(JNIEnv* env);
	static jobject GetCurrentScreen(JNIEnv* env);
	static jobject GetRenderManager(JNIEnv* env);
	static jobject GetTimer(JNIEnv* env);
	static jobject GetFontRenderer(JNIEnv* env);
	static jobject GetObjectMouseOver(JNIEnv* env);
	static jobject GetGameSettings(JNIEnv* env);
	static int GetDisplayHeight(JNIEnv* env);
	static int GetDisplayWidth(JNIEnv* env);
};