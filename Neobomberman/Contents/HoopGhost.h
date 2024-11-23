#pragma once
#include "Monster.h"

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
	void InitSprite() override;
	void ChangeMoveAnim(const FVector2D& _direction) override;

private:
	void InitMoveEllipse();

	/* Fsm update functions */
	void PressingDown(float _deltaTime);
	void Walking(float _deltaTime) override;

	/* Fsm start functions */
	void OnWalk() override;

	const char* SPRITE_NAME = "HoopGhost.png";

	const char* ANIM_START_HOOP = "Start_Hoop";
	const char* ANIM_RUN_HOOP = "Run_Hoop";
	const char* ANIM_RUN_DOWN = "Run_Down";

	std::vector<int> EllipseXPts;
	std::vector<int> EllipseYPts;
	int InitRoundingIdx = 0;
	int EllipsePtrSize = 0;
	int RoundingIdx = 0;
	FVector2D CenterLoc;
};
