#pragma once
#include "Monster.h"
#include <EngineBase/EngineRandom.h>
#include <EngineBase/FSMStateManager.h>

enum class EMushroomState
{
	INIT_BLINK = 0,
	INIT_WALKING,
	THINKING,
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
	FVector2D GetDirection(const FIntPoint& _vec);
	void Move(const FVector2D& direction, float _deltaTime);
	bool IsJump();
	bool IsArrivedAtOneBlock();
	void FindPath();

	/* FSM functions */
	void Blinking(float _deltaTime);
	void WalkingForStart(float _deltaTime);
	void Thinking(float _deltaTime);
	void Walking(float _deltaTime);
	void Jumping(float _deltaTime);

	const float BLINK_SECONDS = 1.f;
	const char* SPRITE_NAME = "Mushroom.png";
	const int IDX_ANIM_START = 44;
	const int IDX_ANIM_END = 54;

	UFSMStateManager FSM;
	UEngineRandom Random;
	FIntPoint FirstIdx;
};
