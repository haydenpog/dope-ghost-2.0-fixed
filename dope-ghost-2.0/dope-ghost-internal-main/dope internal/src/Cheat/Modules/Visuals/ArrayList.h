#pragma once
#include "../Module.h"

class ArrayList : public Module
{
	virtual void OnImGuiRender(JNIEnv* env) override;
};