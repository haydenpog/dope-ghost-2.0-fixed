#pragma once
#include <vector>

class Player;
class World
{
public:
	std::vector<Player*> GetPlayerEntities(JNIEnv* env);
};