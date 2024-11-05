#pragma once
#include "EngineCore/Actor.h"

class AEnemy : public AActor
{
public:
	AEnemy();
	~AEnemy();

	AEnemy(const AEnemy& _other) = delete;
	AEnemy(AEnemy&& _other) noexcept = delete;
	AEnemy& operator=(const AEnemy& _other) = delete;
	AEnemy& operator=(AEnemy&& _other) noexcept = delete;
};
