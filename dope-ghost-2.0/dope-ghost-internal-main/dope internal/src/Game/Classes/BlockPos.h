#pragma once

class BlockPos
{
public:
	static jobject GetBlock(Vec3D pos, jobject worldObject, JNIEnv* env);
};