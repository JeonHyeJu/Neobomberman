#pragma once
#include "Monster.h"

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

	void InitSprite() override;
	void ChangeMoveAnim(const FVector2D& _direction) override;

private:
	const char* SPRITE_NAME = "Balloon_long.png";

	const char* ANIM_RUN_UP = "Run_Up";
	const char* ANIM_RUN_DOWN = "Run_Down";
	const char* ANIM_RUN_LEFT = "Run_Left";
	const char* ANIM_RUN_RIGHT = "Run_Right";
};