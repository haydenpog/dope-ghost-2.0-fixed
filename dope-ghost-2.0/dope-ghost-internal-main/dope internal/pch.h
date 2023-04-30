#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#include <memory>
#include <string>
#include <jni.h>
#include <mutex>
#include <algorithm>
#include <numbers>

#include "src/Helper/Vec3.h"
#include "src/Helper/Vec2.h"
#include "src/Helper/Vec4.h"

#define DOPE_DEBUG _DEBUG

enum HackState
{
	ERR,
	INCOMPLETE,
	WORKING,
	SUCCESS
};