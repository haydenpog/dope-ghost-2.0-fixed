#pragma once

class FontRenderer
{
public:
	int GetStringWidth(const char* str, JNIEnv* env);
	void DrawString(const char* str, float x, float y, char dropShadow, int color, JNIEnv* env);
	/*
		color	p_175065_4_	I
		dropShadow	p_175065_5_	Z
		text	p_175065_1_	Ljava/lang/String;
		x	p_175065_2_	F
		y	p_175065_3_	F
	*/
};