#pragma once
#include "Monster.h"
#include <EngineBase/EngineRandom.h>
#include <EngineBase/FSMStateManager.h>

class AMushroom : public AMonster
{
public:
	AMushroom();
	~AMushroom();

	AMushroom(const AMushroom& _other) = delete;
	AMushroom(AMushroom&& _other) noexcept = delete;
	AMushroom& operator=(const AMushroom& _other) = delete;
	AMushroom& operator=(AMushroom&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void Kill() override;
	void Damaged(unsigned __int8 _power) override;
	FVector2D GetMonsterSize() override;
	FIntPoint GetDamageRange() override;

	void InitSprite();
	void InitCollision();
	void ChangeMoveAnim(const FVector2D& direction);

private:
	void Move(const FVector2D& direction, float _deltaTime);

	/* FSM update callbacks */
	/* You don't need to call Super::function()! */
	void Blinking(float _deltaTime);
	void WalkingForStart(float _deltaTime);
	void Thinking(float _deltaTime);
	void Walking(float _deltaTime);
	void Damaging(float _deltaTime);
	void Dying(float _deltaTime);
	void PassAwaing(float _deltaTime);

	void OnPause() override;
	void OnResume() override;

	/* FSM start callbacks */
	void OnDead();
	void OnPassaway();

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

	const char* ANIM_CLOUD = "Show_Cloud";
	const char* ANIM_SCORE = "Show_Score";

	const char* ANIM_JUMP = "Jump";

	UEngineRandom Random;

	USpriteRenderer* SRBody = nullptr;
	USpriteRenderer* SRCloud = nullptr;
	USpriteRenderer* SRScore = nullptr;

	class U2DCollision* Collision = nullptr;

	UFSMStateManager Fsm;

	bool IsInited = false;
};
