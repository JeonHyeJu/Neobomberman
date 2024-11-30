#pragma once
#include "BaseGameMode.h"

class APlayStage1Mode : public ABaseGameMode
{
public:
	APlayStage1Mode();
	~APlayStage1Mode();

	APlayStage1Mode(const APlayStage1Mode& _Other) = delete;
	APlayStage1Mode(APlayStage1Mode&& _Other) noexcept = delete;
	APlayStage1Mode& operator=(const APlayStage1Mode& _Other) = delete;
	APlayStage1Mode& operator=(APlayStage1Mode&& _Other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void FinishGame() override;

private:
	void CheckAndPlayBgSound();

	class AGameUI* GameUiPtr = nullptr;

	const int MONSTER_CNT_STAGE_1 = 4;
	const FIntPoint PORTAL_IDX_STAGE_1 = { 6, 10 };

	/* Sounds */
	const char* SFXBg = "Stage1Music.mp3";
	const char* SFXBgHurryUp = "HurryUpMusic.mp3";
	const char* SFXAlertHurryUp = "HurryUpWarning.mp3";
	const char* SFXOpenPortal = "OpenPortal.mp3";

	float ElapsedSecs = 0.f;
};