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

	void Init() override;

private:
	const char* SPRITE_NAME = "Balloon_long.png";
};