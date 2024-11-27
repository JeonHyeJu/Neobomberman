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

	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void Damaged(unsigned __int8 _power) override;
	void Kill() override;

	FVector2D GetMonsterSize() override;
	FIntPoint GetDamageRange() override;

private:
	void InitMoveEllipse();
	void InitSprite();
	void InitCollision();
	void ChangeMoveAnim(const FVector2D& _direction);

	void toggleShadow();

	/* Fsm update functions */
	void Walking(float _deltaTime);
	void PressingDown(float _deltaTime);
	void Damaging(float _deltaTime);
	void Dying(float _deltaTime);
	void PassAwaing(float _deltaTime);

	/* Fsm start functions */
	void OnWalk();
	void OnPassaway();
	void OnPressDown();
	void OnDamage();

	// for debug start
	const char* DEBUG_IMG_PATH = "Bg_1-Col.png";
	USpriteRenderer* DebugRenderer = nullptr;
	// for debug end

	const char* SPRITE_NAME = "HoopGhost.png";
	const char* SHADOW_S_SPRITE_NAME = "HoopGhostShadow_S.png";
	const char* SHADOW_L_SPRITE_NAME = "HoopGhostShadow_L.png";
	const char* DETROY_SPRITE_PATH = "HoopGhostCloud";

	const char* ANIM_START_HOOP = "Start_Hoop";
	const char* ANIM_RUN_HOOP = "Run_Hoop";
	const char* ANIM_START_PRESS = "Start_Press";
	const char* ANIM_RUN_PRESS = "Run_Press";
	const char* ANIM_DAMAGED = "Damaged";
	const char* ANIM_DYING = "Dying";
	const char* ANIM_DESTROY = "Destroy";

	std::vector<int> EllipseXPts;
	std::vector<int> EllipseYPts;
	int InitRoundingIdx = 0;
	int EllipsePtrSize = 0;
	int RoundingIdx = 0;
	FVector2D CenterLoc;
	FVector2D BottomLoc;
	FVector2D SavedLoc;
	FVector2D SavedShadowLoc;

	float PadToBottomSize = 0.f;
	float ElapsedSesc = 0.f;

	FVector2D DetectShape = { 120, 64 };

	const float WALKING_DELAY = .03f;
	const float WAIT_DELAY = WALKING_DELAY * 200;
	float DelaySecs = WAIT_DELAY;

	USpriteRenderer* SRBody = nullptr;
	USpriteRenderer* SRShadowS = nullptr;
	USpriteRenderer* SRShadowL = nullptr;
	USpriteRenderer* SRCloud = nullptr;
	U2DCollision* Collision = nullptr;

	UFSMStateManager Fsm;

	float BodyHHY = 0.f;	// Body Half Half Y
	const float SHADOW_MARGIN = 8.f;

	const int MAX_HEALTH = 1;	// Temp

	const FVector2D MONSTER_SIZE{ 256, 256 };
};
