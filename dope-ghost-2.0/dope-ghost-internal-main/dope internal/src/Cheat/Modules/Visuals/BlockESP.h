#pragma once
#include "../Module.h"

class BlockESP : public Module
{
public:
	virtual void Run(JNIEnv* env) override;
	virtual void OnRender(JNIEnv* env) override;
};