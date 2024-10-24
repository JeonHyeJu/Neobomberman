#pragma once
#include "EngineCore/Actor.h"

class Enemy : public AActor
{
public:
	Enemy();
	~Enemy();

	Enemy(const Enemy& _other) = delete;
	Enemy(Enemy&& _other) = delete;
	Enemy& operator=(const Enemy& _other) = delete;
	Enemy& operator=(Enemy&& _other) noexcept = delete;
};
