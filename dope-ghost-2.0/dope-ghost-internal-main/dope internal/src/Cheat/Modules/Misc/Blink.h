#pragma once
#include "../Module.h"

class Blink : public Module
{
	virtual void Run(JNIEnv* env) override;
	virtual void OnRender(JNIEnv* env) override;
	virtual void OnImGuiRender(JNIEnv* env) override;
	virtual void OnReceiveData() override;
};