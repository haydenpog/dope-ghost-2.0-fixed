#pragma once
#include "../Module.h"

class ESP : public Module
{
public:
	virtual void Run(JNIEnv* env) override;
	virtual void OnRender(JNIEnv* env) override;
	virtual void OnImGuiRender(JNIEnv* env) override;
};