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
	void FinishGame() override;

private:
	FIntPoint StartPoint = { 6, 8 };

	class AGameUI* GameUiPtr = nullptr;

	float ElapsedSecs = 0.f;

	/* Sounds */
	const char* SFXBg = "Stage1Boss.mp3";
};
