#pragma once
#include "Monster.h"

class USpriteRenderer;
class U2DCollision;
class AHoopGhost : public AMonster
{
public:
	AHoopGhost();
	~AHoopGhost();

	AHoopGhost(const AHoopGhost& _other) = delete;
	AHoopGhost(AHoopGhost&& _other) noexcept = delete;
	AHoopGhost& operator=(const AHoopGhost& _other) = delete;
	AHoopGhost& operator=(AHoopGhost&& _other) noexcept = delete;

	void Damaged(unsigned __int8 _power) override;
	void Kill() override;
	bool IsDying() override;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	void InitSprite();
	void InitCollision();
	void InitMoveEllipse();

	void ChangeMoveAnim(const FVector2D& _direction);
	void toggleShadow();
	void GoingUp();
	void SetSpeed2x();

	/* Fsm start functions */
	void OnWalk();
	void OnPressDown();
	void OnDamage();
	void OnPassaway();

	/* Fsm update functions */
	void WaitingStartDelay(float _deltaTime);
	void Walking(float _deltaTime);
	void PressingDown(float _deltaTime);
	void Damaging(float _deltaTime);
	void Dying(float _deltaTime);
	void PassAwaing(float _deltaTime);

	const char* SPRITE_NAME = "HoopGhost.png";

	const char* ANIM_START_HOOP = "Start_Hoop";
	const char* ANIM_RUN_HOOP = "Run_Hoop";
	const char* ANIM_START_PRESS = "Start_Press";
	const char* ANIM_RUN_PRESS = "Run_Press";
	const char* ANIM_AFTER_PRESS = "After_Press";
	const char* ANIM_DAMAGED = "Damaged";
	const char* ANIM_DYING = "Dying";
	const char* ANIM_DESTROY = "Destroy";

	USpriteRenderer* SRBody = nullptr;
	USpriteRenderer* SRShadowS = nullptr;
	USpriteRenderer* SRShadowL = nullptr;
	USpriteRenderer* SRCloud = nullptr;
	U2DCollision* Collision = nullptr;

	UFSMStateManager Fsm;

	/* Rounding */
	std::vector<int> EllipseXPts;
	std::vector<int> EllipseYPts;
	int EllipsePtSize = 0;
	int RoundingIdx = 0;
	FVector2D CenterLoc;
	FVector2D BottomLoc;
	FVector2D SavedLoc;
	float PadToBottomSize = 0.f;
	float MoveSize = 0.f;

	float ElapsedSesc = 0.f;

	FVector2D DetectShape = { 120, 64 };

	float WalkingDelay = .03f;
	float WaitDelay = WalkingDelay * 200;
	float DelaySecs = WaitDelay;

	float BodyHHY = 0.f;	// Body Half Half Y
	const float SHADOW_MARGIN = 8.f;

	const int MAX_HEALTH = 3;

	/* Sounds */
	const char* SFXHoop = "Hoop.mp3";
	const char* SFXCrash = "Crash.mp3";

	float ExplosionSecs = 0.f;
	float BlinkElapsedSecs = 0.f;
};
