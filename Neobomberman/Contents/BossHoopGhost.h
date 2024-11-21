#pragma once
#include "Monster.h"

class ABossHoopGhost : public AMonster
{
public:
	ABossHoopGhost();
	~ABossHoopGhost();

	ABossHoopGhost(const ABossHoopGhost& _other) = delete;
	ABossHoopGhost(ABossHoopGhost&& _other) noexcept = delete;
	ABossHoopGhost& operator=(const ABossHoopGhost& _other) = delete;
	ABossHoopGhost& operator=(ABossHoopGhost&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void InitSprite() override;

private:
	const char* SPRITE_NAME = "";
};
