#include "pch.h"
#include "Player.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

std::string Player::GetName(JNIEnv* env, bool shouldEraseColor)
{
	if (this == NULL || !env)
		return "";

	const auto playerClazz = (Klass*)env->GetObjectClass((jclass)this);
	if (!playerClazz)
		return "";

	const auto getEntityStringMethod = playerClazz->GetMethod(env, Mapper::Get("getDisplayName").data(), Mapper::Get("net/minecraft/util/IChatComponent", 3).data());
	const auto entityStringObject = getEntityStringMethod->CallObjectMethod(env, (jobject)this);
	const auto iChatComponentClazz = g_Instance->FindClass(Mapper::Get("net/minecraft/util/IChatComponent"));
	const auto getFormattedTextMethod = iChatComponentClazz->GetMethod(env, Mapper::Get("getUnformattedTextForChat").data(), "()Ljava/lang/String;");
	const auto formattedTextObject = getFormattedTextMethod->CallObjectMethod(env, entityStringObject);
	const auto formattedText = (jstring)formattedTextObject;
	const auto entityName = env->GetStringUTFChars(formattedText, NULL);
	auto ret = std::string(entityName);

	if (formattedTextObject)
		env->ReleaseStringUTFChars(formattedText, entityName);

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	if (entityStringObject)
		env->DeleteLocalRef(entityStringObject);

	if (formattedText)
		env->DeleteLocalRef(formattedText);

	auto getLastIndex = [](char* s, char c)
	{
		std::vector<int> indexes;
		int length;
		int i;

		length = (int)strlen(s);

		for (i = (length - 1); i >= 0; i--)
		{
			if (s[i] == c)
				indexes.push_back(i);
		}

		return indexes;
	};

	if (shouldEraseColor)
		for (auto index : getLastIndex(ret.data(), '\u00A7'))
			ret.erase(index - 1, 3);

	return ret;
}

float Player::GetRotationPitch(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto pitchField = playerClazz->GetField(env, Mapper::Get("rotationPitch").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return pitchField->GetFloatField(env, this);
}

float Player::GetRotationYaw(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto pitchField = playerClazz->GetField(env, Mapper::Get("rotationYaw").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return pitchField->GetFloatField(env, this);
}

float Player::GetRotationYawHead(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto pitchField = playerClazz->GetField(env, Mapper::Get("rotationYawHead").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return pitchField->GetFloatField(env, this);
}

float Player::GetPrevRotationPitch(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto pitchField = playerClazz->GetField(env, Mapper::Get("prevRotationPitch").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return pitchField->GetFloatField(env, this);
}

float Player::GetPrevRotationYaw(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;
	
	const auto pitchField = playerClazz->GetField(env, Mapper::Get("prevRotationYaw").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return pitchField->GetFloatField(env, this);
}

float Player::GetPrevRenderYawOffset(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto prevRenderYawOffsetField = playerClazz->GetField(env, Mapper::Get("prevRenderYawOffset").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return prevRenderYawOffsetField->GetFloatField(env, this);
}

float Player::GetRenderYawOffset(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto yawOffsetField = playerClazz->GetField(env, Mapper::Get("renderYawOffset").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return yawOffsetField->GetFloatField(env, this);
}

Vec3D Player::GetLastTickPos(JNIEnv* env)
{
	if (this == NULL || !env)
		return Vec3D();

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return Vec3D();

	const auto pitchFieldX = playerClazz->GetField(env, Mapper::Get("lastTickPosX").data(), "D");
	const auto pitchFieldY = playerClazz->GetField(env, Mapper::Get("lastTickPosY").data(), "D");
	const auto pitchFieldZ = playerClazz->GetField(env, Mapper::Get("lastTickPosZ").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return { pitchFieldX->GetDoubleField(env, this), 
		pitchFieldY->GetDoubleField(env, this), 
		pitchFieldZ->GetDoubleField(env, this)
	};
}

Vec3D Player::GetPos(JNIEnv* env)
{
	if (this == NULL || !env)
		return Vec3D();

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return Vec3D();

	const auto posFieldX = playerClazz->GetField(env, Mapper::Get("posX").data(), "D");
	const auto posFieldY = playerClazz->GetField(env, Mapper::Get("posY").data(), "D");
	const auto posFieldZ = playerClazz->GetField(env, Mapper::Get("posZ").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return { posFieldX->GetDoubleField(env, this), 
		posFieldY->GetDoubleField(env, this), 
		posFieldZ->GetDoubleField(env, this) 
	};
}

Vec3D Player::GetPreviousPos(JNIEnv* env)
{
	if (this == NULL || !env)
		return Vec3D();

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return Vec3D();

	const auto posFieldX = playerClazz->GetField(env, Mapper::Get("prevPosX").data(), "D");
	const auto posFieldY = playerClazz->GetField(env, Mapper::Get("prevPosY").data(), "D");
	const auto posFieldZ = playerClazz->GetField(env, Mapper::Get("prevPosZ").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return { posFieldX->GetDoubleField(env, this), 
		posFieldY->GetDoubleField(env, this),
		posFieldZ->GetDoubleField(env, this) 
	};
}

double Player::GetMotionX(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0;

	const auto motionField = playerClazz->GetField(env, Mapper::Get("motionX").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return motionField->GetDoubleField(env, this);
}

double Player::GetMotionY(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0;

	const auto motionField = playerClazz->GetField(env, Mapper::Get("motionY").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return motionField->GetDoubleField(env, this);
}

double Player::GetMotionZ(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0;

	const auto motionField = playerClazz->GetField(env, Mapper::Get("motionZ").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return motionField->GetDoubleField(env, this);
}

float Player::GetHealth(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto getHealthMethod = playerClazz->GetMethod(env, Mapper::Get("getHealth").data(), "()F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return getHealthMethod->CallFloatMethod(env, (jobject)this);
}

float Player::GetMoveForward(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto moveField = playerClazz->GetField(env, Mapper::Get("moveForward").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return moveField->GetFloatField(env, this);
}

float Player::GetMoveStrafing(JNIEnv* env)
{
	if (this == NULL || !env)
		return 0.0f;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return 0.0f;

	const auto moveField = playerClazz->GetField(env, Mapper::Get("moveStrafing").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return moveField->GetFloatField(env, this);
}

void Player::SetAlwaysRenderNameTag(bool state, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return;

	const auto setAlwaysRenderNametagMethod = playerClazz->GetMethod(env, Mapper::Get("setAlwaysRenderNameTag").data(), "(Z)V");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return setAlwaysRenderNametagMethod->CallVoidMethod(env, (jobject)this, false, state);
}

bool Player::IsNPC(JNIEnv* env)
{
	if (this == NULL || !env)
		return true;

	const std::string playerName = this->GetName(env);
	/*if (playerName.empty() || playerName.size() <= 1) { forge 1.8 no name even tho methods are corrects
		return true;
	}*/

	if (playerName.starts_with("\u00a7") && playerName.size() <= 2)
		return true;

	if (playerName.find("[NPC]") != std::string::npos) {
		return  true;
	}

	if (!playerName.starts_with("\u00a7") && playerName.ends_with("\u00a7r") || playerName.starts_with("\247")) {
		return true;
	}

	if (playerName.starts_with("\u00a7") && playerName.find("\u00a7c") != std::string::npos) {
		return true;
	}

	if (this->IsInvisible(env)) { // antibot
		return true;
	}

	return false;
}

bool Player::IsInvisible(JNIEnv* env)
{
	if (this == NULL || !env)
		return true;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return true;

	const auto isInvisibleMethod = playerClazz->GetMethod(env, Mapper::Get("isInvisible").data(), "()Z");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return isInvisibleMethod->CallBoolMethod(env, (jobject)this);
}

bool Player::IsSneaking(JNIEnv* env)
{
	if (this == NULL || !env)
		return true;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return true;

	const auto isInvisibleMethod = playerClazz->GetMethod(env, Mapper::Get("isSneaking").data(), "()Z");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return isInvisibleMethod->CallBoolMethod(env, (jobject)this);
}

bool Player::IsOnGround(JNIEnv* env)
{
	if (this == NULL || !env)
		return NULL;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return NULL;

	const auto hurtField = playerClazz->GetField(env, Mapper::Get("onGround").data(), "Z");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return hurtField->GetBooleanField(env, this);
}

bool Player::IsInWater(JNIEnv* env)
{
	if (this == NULL || !env)
		return NULL;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return NULL;

	const auto hurtField = playerClazz->GetField(env, Mapper::Get("inWater").data(), "Z");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return hurtField->GetBooleanField(env, this);
}

int Player::GetMaxHurtResistantTime(JNIEnv* env)
{
	if (this == NULL || !env)
		return NULL;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return NULL;

	const auto hurtField = playerClazz->GetField(env, Mapper::Get("maxHurtResistantTime").data(), "I");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return hurtField->GetIntField(env, this);
}

int Player::GetHurtResistantTime(JNIEnv* env)
{
	if (this == NULL || !env)
		return NULL;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return NULL;

	const auto hurtField = playerClazz->GetField(env, Mapper::Get("hurtResistantTime").data(), "I");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return hurtField->GetIntField(env, this);
}

jobject Player::GetBoundingBox(JNIEnv* env)
{
	if (this == NULL || !env)
		return NULL;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return NULL;

	const auto boundingBoxField = playerClazz->GetField(env, Mapper::Get("boundingBox").data(), Mapper::Get("net/minecraft/util/AxisAlignedBB", 2).data());

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return boundingBoxField->GetObjectField(env, this);
}

jobject Player::GetHeldItem(JNIEnv* env)
{
	if (this == NULL || !env)
		return NULL;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return NULL;

	const auto getHeldItemMethod = playerClazz->GetMethod(env, Mapper::Get("getHeldItem").data(), Mapper::Get("net/minecraft/item/ItemStack", 3).data());
	
	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return getHeldItemMethod->CallObjectMethod(env, (jobject)this);
}

jobject Player::GetInventoryPlayer(JNIEnv* env)
{
	if (this == NULL || !env)
		return NULL;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return NULL;

	const auto inventoryContainerField = playerClazz->GetField(env, Mapper::Get("inventory").data(), Mapper::Get("net/minecraft/entity/player/InventoryPlayer", 2).data());

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	return inventoryContainerField->GetObjectField(env, this);
}

void Player::SetMotionX(double buffer, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return;

	const auto motionField = playerClazz->GetField(env, Mapper::Get("motionX").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	motionField->SetDoubleField(env, this, buffer);
}

void Player::SetMotionY(double buffer, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return;

	const auto motionField = playerClazz->GetField(env, Mapper::Get("motionY").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	motionField->SetDoubleField(env, this, buffer);
}

void Player::SetMotionZ(double buffer, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return;

	const auto motionField = playerClazz->GetField(env, Mapper::Get("motionZ").data(), "D");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	motionField->SetDoubleField(env, this, buffer);
}

void Player::SetRotationPitch(float buffer, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return;

	const auto pitchField = playerClazz->GetField(env, Mapper::Get("rotationPitch").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	pitchField->SetFloatField(env, this, buffer);
}

void Player::SetRotationYaw(float buffer, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return;

	const auto pitchField = playerClazz->GetField(env, Mapper::Get("rotationYaw").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	pitchField->SetFloatField(env, this, buffer);
}

void Player::SetPrevRotationPitch(float buffer, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return;

	const auto pitchField = playerClazz->GetField(env, Mapper::Get("prevRotationPitch").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	pitchField->SetFloatField(env, this, buffer);
}

void Player::SetPrevRotationYaw(float buffer, JNIEnv* env)
{
	if (this == NULL || !env)
		return;

	const auto playerClazz = (Klass*)env->GetObjectClass((jobject)this);
	if (!playerClazz)
		return;

	const auto pitchField = playerClazz->GetField(env, Mapper::Get("prevRotationYaw").data(), "F");

	if (playerClazz)
		env->DeleteLocalRef((jclass)playerClazz);

	pitchField->SetFloatField(env, this, buffer);
}
