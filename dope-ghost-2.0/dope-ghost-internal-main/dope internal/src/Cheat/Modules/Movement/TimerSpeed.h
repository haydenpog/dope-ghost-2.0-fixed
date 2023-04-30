#pragma once
#include "../Module.h"

class TimerSpeed : public Module
{
public:
	virtual void Run(JNIEnv* env) override;
};