#pragma once

class Player
{
public:
	std::string GetName(JNIEnv* env, bool shouldEraseColor = false);
	float GetRotationPitch(JNIEnv* env);
	float GetRotationYaw(JNIEnv* env);
	float GetRotationYawHead(JNIEnv* env);
	float GetPrevRotationPitch(JNIEnv* env);
	float GetPrevRotationYaw(JNIEnv* env);
	float GetPrevRenderYawOffset(JNIEnv* env);
	float GetRenderYawOffset(JNIEnv* env);
	float GetHealth(JNIEnv* env);
	float GetMoveForward(JNIEnv* env);
	float GetMoveStrafing(JNIEnv* env);
	Vec3D GetLastTickPos(JNIEnv* env);
	Vec3D GetPos(JNIEnv* env);
	Vec3D GetPreviousPos(JNIEnv* env);
	double GetMotionX(JNIEnv* env);
	double GetMotionY(JNIEnv* env);
	double GetMotionZ(JNIEnv* env);
	bool IsNPC(JNIEnv* env);
	bool IsInvisible(JNIEnv* env);
	bool IsSneaking(JNIEnv* env);
	bool IsOnGround(JNIEnv* env);
	bool IsInWater(JNIEnv* env);
	int GetMaxHurtResistantTime(JNIEnv* env);
	int GetHurtResistantTime(JNIEnv* env);
	jobject GetBoundingBox(JNIEnv* env);
	jobject GetHeldItem(JNIEnv *env);
	jobject GetInventoryPlayer(JNIEnv* env);

	void SetAlwaysRenderNameTag(bool state, JNIEnv* env);
	void SetMotionX(double buffer, JNIEnv* env);
	void SetMotionY(double buffer, JNIEnv* env);
	void SetMotionZ(double buffer, JNIEnv* env);
	void SetRotationPitch(float buffer, JNIEnv* env);
	void SetRotationYaw(float buffer, JNIEnv* env);
	void SetPrevRotationPitch(float buffer, JNIEnv* env);
	void SetPrevRotationYaw(float buffer, JNIEnv* env);
};