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
	const char* SPRITE_NAME = "HoopGhost.png";

	const char* ANIM_START_HOOP = "Start_Hoop";
	const char* ANIM_RUN_HOOP = "Run_Hoop";
	const char* ANIM_RUN_DOWN = "Run_Down";
};
