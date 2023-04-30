#pragma once
#include "../Module.h"

class Bhop : public Module
{
public:
	virtual void Run(JNIEnv* env) override;
};