#include "pch.h"
#include "World.h"
#include "Player.h"

#include "../Klass.h"
#include "../Field.h"
#include "../Method.h"
#include "../Mapper.h"

#include "../../Cheat/Hack.h"

std::vector<Player*> World::GetPlayerEntities(JNIEnv* env)
{
	if (this == NULL)
		return {};
	
	const auto worldClazz = (Klass*)env->GetObjectClass((jobject)this);
	const auto playerEntitiesField = worldClazz->GetField(env, Mapper::Get("playerEntities").data(), "Ljava/util/List;");
	const auto listClazz = (Klass*)g_Instance->FindClass("java/util/List");
	const auto toArrayMethod = listClazz->GetMethod(env, "toArray", "()[Ljava/lang/Object;");

	const auto playerEntitiesObject = playerEntitiesField->GetObjectField(env, (jobject)this);
	const auto playerListArray = (jobjectArray)toArrayMethod->CallObjectMethod(env, playerEntitiesObject);
	if (!playerListArray)
		return {};

	const auto  playerListSize = env->GetArrayLength(playerListArray);

	std::vector<Player*> ret;
	for (int i = 0; i < playerListSize; i++)
	{
		auto pl = (Player*)env->GetObjectArrayElement(playerListArray, i);
		if (!pl)
			continue;

		if (pl->IsNPC(env))
			continue;

		ret.push_back(pl);
	}

	if (playerEntitiesObject)
		env->DeleteLocalRef(playerEntitiesObject);

	if (playerListArray)
		env->DeleteLocalRef(playerListArray);

	return ret;
}
