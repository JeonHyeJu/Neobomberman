#pragma once
#include <EngineCore/Actor.h>

class USpriteRenderer;
class AGameOver : public AActor
{
public:
	AGameOver();
	~AGameOver();

	AGameOver(const AGameOver& _other) = delete;
	AGameOver(AGameOver&& _other) noexcept = delete;
	AGameOver& operator=(const AGameOver& _other) = delete;
	AGameOver& operator=(AGameOver&& _other) noexcept = delete;

	void ShowAndStart();
	void Reset();
	inline bool GetIsOver() const
	{
		return IsOver;
	}

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	void Countdown(float _deltaTime);
	void GameOverStrAnim();
	void GameOver();

	USpriteRenderer* SRContinueStr = nullptr;
	USpriteRenderer* SRContinueNum = nullptr;
	USpriteRenderer* SRGameOverStrs[8] = { nullptr, };

	const char* GAMEOVER_PATH = "GameOver.png";

	const int WAIT_SECONDS = 10;	// 10 secs
	float ElapsedSecs = 0.f;
	int CurSeconds = 0;
	int GameoverSeconds = 0;
	bool IsOver = false;

	float TIME_CNT = 1.f;

	/* Sounds */
	const char* SFXContinue = "Continue.mp3";
	const char* SFXGameOver = "GameOver.mp3";
	const char* SFXBonusCount = "BonusCount.mp3";
};
