#include "pch.h"
#include "Field.h"

jobject Field::GetObjectField(JNIEnv* env, void* fieldOwner, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return NULL;

	return staticField ? env->GetStaticObjectField((jclass)fieldOwner, (jfieldID)this) : env->GetObjectField((jobject)fieldOwner, (jfieldID)this);
}

float Field::GetFloatField(JNIEnv* env, void* fieldOwner, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return NULL;

	return staticField ? env->GetStaticFloatField((jclass)fieldOwner, (jfieldID)this) : env->GetFloatField((jobject)fieldOwner, (jfieldID)this);
}

double Field::GetDoubleField(JNIEnv* env, void* fieldOwner, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return NULL;

	return staticField ? env->GetStaticDoubleField((jclass)fieldOwner, (jfieldID)this) : env->GetDoubleField((jobject)fieldOwner, (jfieldID)this);
}

int Field::GetIntField(JNIEnv* env, void* fieldOwner, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return NULL;

	return staticField ? env->GetStaticIntField((jclass)fieldOwner, (jfieldID)this) : env->GetIntField((jobject)fieldOwner, (jfieldID)this);
}

bool Field::GetBooleanField(JNIEnv* env, void* fieldOwner, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return NULL;

	return staticField ? env->GetStaticBooleanField((jclass)fieldOwner, (jfieldID)this) : env->GetBooleanField((jobject)fieldOwner, (jfieldID)this);
}

void Field::SetObjectField(JNIEnv* env, void* fieldOwner, jobject buffer, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return;

	staticField ? env->SetStaticObjectField((jclass)fieldOwner, (jfieldID)this, buffer) : env->SetObjectField((jobject)fieldOwner, (jfieldID)this, buffer);
}

void Field::SetFloatField(JNIEnv* env, void* fieldOwner, float buffer, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return;

	staticField ? env->SetStaticFloatField((jclass)fieldOwner, (jfieldID)this, buffer) : env->SetFloatField((jobject)fieldOwner, (jfieldID)this, buffer);
}

void Field::SetDoubleField(JNIEnv* env, void* fieldOwner, double buffer, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return;

	staticField ? env->SetStaticDoubleField((jclass)fieldOwner, (jfieldID)this, buffer) : env->SetDoubleField((jobject)fieldOwner, (jfieldID)this, buffer);
}

void Field::SetIntField(JNIEnv* env, void* fieldOwner, int buffer, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return;

	staticField ? env->SetStaticIntField((jclass)fieldOwner, (jfieldID)this, buffer) : env->SetIntField((jobject)fieldOwner, (jfieldID)this, buffer);
}

void Field::SetBooleanField(JNIEnv* env, void* fieldOwner, bool buffer, bool staticField)
{
	if (this == NULL || fieldOwner == NULL || env == NULL)
		return;

	staticField ? env->SetStaticBooleanField((jclass)fieldOwner, (jfieldID)this, buffer) : env->SetBooleanField((jobject)fieldOwner, (jfieldID)this, buffer);
}
