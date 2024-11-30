#pragma once
#include "BaseGameMode.h"

class APlayBossMode : public ABaseGameMode
{
public:
	APlayBossMode();
	~APlayBossMode();

	APlayBossMode(const APlayBossMode& _other) = delete;
	APlayBossMode(APlayBossMode&& _other) noexcept = delete;
	APlayBossMode& operator=(const APlayBossMode& _other) = delete;
	APlayBossMode& operator=(APlayBossMode&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void OnFinishGame() override;
	void FinishingGame(float _deltaTime) override;

private:
	FIntPoint PlayerStartPoint = { 6, 8 };
};
