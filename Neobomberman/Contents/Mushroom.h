#pragma once
#include "Monster.h"

enum class EMushroomState
{
	INIT_BLINK = 0,
	WALKING,
	JUMPING,
};

class AMushroom : public AMonster
{
public:
	AMushroom();
	~AMushroom();

	AMushroom(const AMushroom& _other) = delete;
	AMushroom(AMushroom&& _other) noexcept = delete;
	AMushroom& operator=(const AMushroom& _other) = delete;
	AMushroom& operator=(AMushroom&& _other) noexcept = delete;

	void Init() override;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	void Walk();
	void FindPath();

	EMushroomState State = EMushroomState::INIT_BLINK;

	float AccumulatedSecs = 0.f;
	const float BLINK_SECONDS = 1.f;
	const char* SPRITE_NAME = "Mushroom.png";
};
