#pragma once
#include "Monster.h"
#include <EngineBase/FSMStateManager.h>

class ABalloon : public AMonster
{
public:
	ABalloon();
	~ABalloon();

	ABalloon(const ABalloon& _other) = delete;
	ABalloon(ABalloon&& _other) noexcept = delete;
	ABalloon& operator=(const ABalloon& _other) = delete;
	ABalloon& operator=(ABalloon&& _other) noexcept = delete;

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

	/* FSM start callbacks */
	void OnDead();
	void OnPassaway();

	/* FSM update callbacks */
	void WaitingStartDelay(float _deltaTime);
	void WaitingBlinking(float _deltaTime);
	void WalkingForStart(float _deltaTime);
	void Thinking(float _deltaTime);
	void Walking(float _deltaTime);
	void Dying(float _deltaTime);
	void PassAwaing(float _deltaTime);

	/* FsmH update callback */
	void Blinking(float _deltaTime);

	const char* SPRITE_NAME = "Balloon.png";

	const char* ANIM_RUN_UP = "Run_Up";
	const char* ANIM_RUN_DOWN = "Run_Down";
	const char* ANIM_RUN_LEFT = "Run_Left";
	const char* ANIM_RUN_RIGHT = "Run_Right";

	const char* ANIM_CLOUD = "Show_Cloud";
	const char* ANIM_SCORE = "Show_Score";

	USpriteRenderer* SRBody = nullptr;
	USpriteRenderer* SRCloud = nullptr;
	USpriteRenderer* SRScore = nullptr;

	class U2DCollision* Collision = nullptr;

	UFSMStateManager Fsm;
	UFSMStateManager FsmH;

	float ElapsedSesc = 0.f;
	float BlinkElapsedSecs = 0.f;
};