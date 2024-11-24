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

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void Kill() override;
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
	void Dying(float _deltaTime);
	void PassAwaing(float _deltaTime);

	void OnPause() override;
	void OnResume() override;

	/* FSM start callbacks */
	void OnPassaway();

	const char* SPRITE_NAME = "Balloon_long.png";

	const char* ANIM_RUN_UP = "Run_Up";
	const char* ANIM_RUN_DOWN = "Run_Down";
	const char* ANIM_RUN_LEFT = "Run_Left";
	const char* ANIM_RUN_RIGHT = "Run_Right";

	USpriteRenderer* SRBody = nullptr;
	USpriteRenderer* SRCloud = nullptr;
	USpriteRenderer* SRScore = nullptr;

	class U2DCollision* Collision = nullptr;

	UFSMStateManager Fsm;

	bool IsInited = false;
};