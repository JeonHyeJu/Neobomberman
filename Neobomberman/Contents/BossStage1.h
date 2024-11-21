#pragma once
#include "Monster.h"

class BossStage1 : public AMonster
{
public:
	BossStage1();
	~BossStage1();

	BossStage1(const BossStage1& _other) = delete;
	BossStage1(BossStage1&& _other) noexcept = delete;
	BossStage1& operator=(const BossStage1& _other) = delete;
	BossStage1& operator=(BossStage1&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void Init() override;

private:
	const char* SPRITE_NAME = "";
};
