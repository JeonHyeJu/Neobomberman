#pragma once
#include "EngineCore/AActor.h"

class Player : public AActor
{
public:
	Player();
	~Player();

	Player(const Player& _other) = delete;
	Player(Player&& _other) = delete;
	Player& operator=(const Player& _other) = delete;
	Player& operator=(Player&& _other) noexcept = delete;
};
