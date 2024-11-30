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

	void Damaged(unsigned __int8 _power) override;
	void Kill() override;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void OnPause() override;
	void OnResume() override;

private:
	void InitSprite();
	void InitCollision();

	void ChangeMoveAnim(const FVector2D& direction);
	void Move(const FVector2D& direction, float _deltaTime);
	bool IsJump();

	/* FSM start callbacks */
	void OnDead();
	void OnPassaway();

	/* FSM update callbacks */
	void WaitingStartDelay(float _deltaTime);
	void WaitingBlinking(float _deltaTime);
	void WalkingForStart(float _deltaTime);
	void Thinking(float _deltaTime);
	void Walking(float _deltaTime);
	void Jumping(float _deltaTime);
	void PassAwaing(float _deltaTime);

	/* FsmH update callback */
	void Blinking(float _deltaTime);

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
	UFSMStateManager FsmH;

	float ElapsedSesc = 0.f;
	float BlinkElapsedSecs = 0.f;
};
