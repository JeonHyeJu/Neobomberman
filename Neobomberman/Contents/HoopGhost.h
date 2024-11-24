#pragma once
#include "Monster.h"

// 241124 TODO: 몬스터 다 분리후 AMonster 다시 작성, HoopGhost, Shadow actor
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

protected:
	void InitSprite() override;
	void InitCollision() override;
	void ChangeMoveAnim(const FVector2D& _direction) override;

private:
	void InitMoveEllipse();

	/* Fsm update functions */
	void Walking(float _deltaTime) override;
	void PressingDown(float _deltaTime);

	/* Fsm start functions */
	void OnWalk() override;
	void OnPressDown();

	// for debug start
	const char* DEBUG_IMG_PATH = "Bg_1-Col.png";
	class USpriteRenderer* DebugRenderer = nullptr;
	// for debug end

	const char* SPRITE_NAME = "HoopGhost.png";

	const char* ANIM_START_HOOP = "Start_Hoop";
	const char* ANIM_RUN_HOOP = "Run_Hoop";
	const char* ANIM_START_PRESS = "Start_Press";
	const char* ANIM_RUN_PRESS = "Run_Press";

	std::vector<int> EllipseXPts;
	std::vector<int> EllipseYPts;
	int InitRoundingIdx = 0;
	int EllipsePtrSize = 0;
	int RoundingIdx = 0;
	FVector2D CenterLoc;
	FVector2D BottomLoc;
	FVector2D SavedLoc;

	float PadToBottomSize = 0.f;
	float ElapsedSesc = 0.f;
	float DetectSizeHalf = 50.f;

	const float WALKING_DELAY = .05f;
	const float WAIT_DELAY = WALKING_DELAY * 200;
	float DelaySecs = WAIT_DELAY;
};
