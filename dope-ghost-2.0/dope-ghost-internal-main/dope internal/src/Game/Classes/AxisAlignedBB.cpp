#include "pch.h"
#include "AxisAlignedBB.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

AxisAlignedBB_t AxisAlignedBB::GetNativeBoundingBox(JNIEnv* env)
{
    if(this == NULL)
		return AxisAlignedBB_t();

	const auto axisAlignedKlass = (Klass*)env->GetObjectClass((jobject)this);

	const auto minXField = axisAlignedKlass->GetField(env, Mapper::Get("minX").data(), "D"),
		minYField = axisAlignedKlass->GetField(env, Mapper::Get("minY").data(), "D"),
		minZField = axisAlignedKlass->GetField(env, Mapper::Get("minZ").data(), "D");

	const auto maxXField = axisAlignedKlass->GetField(env, Mapper::Get("maxX").data(), "D"),
		maxYField = axisAlignedKlass->GetField(env, Mapper::Get("maxY").data(), "D"),
		maxZField = axisAlignedKlass->GetField(env, Mapper::Get("maxZ").data(), "D");

	if (axisAlignedKlass)
		env->DeleteLocalRef((jclass)axisAlignedKlass);

    return AxisAlignedBB_t{
		(float)minXField->GetDoubleField(env, (jobject)this),
		(float)minYField->GetDoubleField(env, (jobject)this),
		(float)minZField->GetDoubleField(env, (jobject)this),

		(float)maxXField->GetDoubleField(env, (jobject)this),
		(float)maxYField->GetDoubleField(env, (jobject)this),
		(float)maxZField->GetDoubleField(env, (jobject)this),
	};
}

void AxisAlignedBB::SetNativeBoundingBox(AxisAlignedBB_t buffer, JNIEnv* env)
{
	if (this == NULL)
		return;

	const auto axisAlignedKlass = (Klass*)env->GetObjectClass((jobject)this);

	const auto minXField = axisAlignedKlass->GetField(env, Mapper::Get("minX").data(), "D"),
		minYField = axisAlignedKlass->GetField(env, Mapper::Get("minY").data(), "D"),
		minZField = axisAlignedKlass->GetField(env, Mapper::Get("minZ").data(), "D");

	const auto maxXField = axisAlignedKlass->GetField(env, Mapper::Get("maxX").data(), "D"),
		maxYField = axisAlignedKlass->GetField(env, Mapper::Get("maxY").data(), "D"),
		maxZField = axisAlignedKlass->GetField(env, Mapper::Get("maxZ").data(), "D");

	if (axisAlignedKlass)
		env->DeleteLocalRef((jclass)axisAlignedKlass);

	minXField->SetDoubleField(env, (jobject)this, buffer.minX);
	minYField->SetDoubleField(env, (jobject)this, buffer.minY);
	minZField->SetDoubleField(env, (jobject)this, buffer.minZ);

	maxXField->SetDoubleField(env, (jobject)this, buffer.maxX);
	maxYField->SetDoubleField(env, (jobject)this, buffer.maxY);
	maxZField->SetDoubleField(env, (jobject)this, buffer.maxZ);
}
