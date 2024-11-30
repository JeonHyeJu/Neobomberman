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
	void OnFinishGame() override;
	void FinishingGame(float _deltaTime) override;

private:
	const FIntPoint PORTAL_IDX_STAGE_1 = { 6, 10 };

	/* Sounds */
	const char* SFXOpenPortal = "OpenPortal.mp3";
};