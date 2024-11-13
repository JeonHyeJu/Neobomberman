#pragma once
#include "Monster.h"
#include <EngineBase/EngineRandom.h>

enum class EMushroomState
{
	INIT_BLINK = 0,
	INIT_WALKING,
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
	void SetFirstIndex(const FIntPoint& _idx);

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	void Move(const FVector2D& direction, float _deltaTime);
	void Jump();
	void Walk(float _deltaTime);
	void WalkForFirstLoc(float _deltaTime);
	void FindPath();

	void OnEndJump();

	EMushroomState State = EMushroomState::INIT_BLINK;

	float AccumulatedSecs = 0.f;
	const float BLINK_SECONDS = 1.f;
	const char* SPRITE_NAME = "Mushroom.png";

	UEngineRandom Random;
	FIntPoint FirstIdx;
};
