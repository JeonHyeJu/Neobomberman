#pragma once
#include "Monster.h"
#include <EngineBase/EngineRandom.h>

class AMushroom : public AMonster
{
public:
	AMushroom();
	~AMushroom();

	AMushroom(const AMushroom& _other) = delete;
	AMushroom(AMushroom&& _other) noexcept = delete;
	AMushroom& operator=(const AMushroom& _other) = delete;
	AMushroom& operator=(AMushroom&& _other) noexcept = delete;

	void InitSprite() override;
	void ChangeMoveAnim(const FVector2D& direction);

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void Thinking(float _deltaTime) override;

private:
	bool IsJump();
	void Jumping(float _deltaTime);

	const char* SPRITE_NAME = "Mushroom.png";

	const char* ANIM_IDLE_UP = "Idle_Up";
	const char* ANIM_IDLE_DOWN = "Idle_Down";
	const char* ANIM_IDLE_LEFT = "Idle_Left";
	const char* ANIM_IDLE_RIGHT = "Idle_Right";

	const char* ANIM_RUN_UP = "Run_Up";
	const char* ANIM_RUN_DOWN = "Run_Down";
	const char* ANIM_RUN_LEFT = "Run_Left";
	const char* ANIM_RUN_RIGHT = "Run_Right";

	const char* ANIM_JUMP = "Jump";

	UEngineRandom Random;
};
