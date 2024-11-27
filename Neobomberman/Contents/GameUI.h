#pragma once
#include <EngineCore/Actor.h>

class USpriteRenderer;
class AGameUI : public AActor
{
public:
	class StatusTopBarUI
	{
	public:
		USpriteRenderer* SRBar = nullptr;
		USpriteRenderer* SRPushStart = nullptr;
		USpriteRenderer* SRPushStartBr = nullptr;

		USpriteRenderer* SRPlayerScore[8] = { nullptr, };	// 7: temp
		USpriteRenderer* SRTimerCounts[3] = { nullptr, };
		USpriteRenderer* SRTimerColumn = nullptr;
		USpriteRenderer* SRPlayerLife = nullptr;

		const char* SCORE_BAR_ADV_SPRITE_PATH = "ScoreBar_Adventure.png";
		const char* SCORE_PUSH_START_PATH = "PushStart.png";
		const char* SCORE_PUSH_START_BR_PATH = "PushStart_Bright.png";
		const char* SCORE_TIME_COUNT = "TimeCount.png";
		const char* BAR_SCORE_NUMBER_PATH = "BarScoreNumber.png";

		bool IsHidden()
		{
			return !IsShow;
		}

		void Show()
		{
			_SetVisibility(true);
		}
		void Hide()
		{
			_SetVisibility(false);
		}

	private:
		void _SetVisibility(bool _isShow);
		bool IsShow = true;
	};

	AGameUI();
	~AGameUI();

	AGameUI(const AGameUI& _other) = delete;
	AGameUI(AGameUI&& _other) noexcept = delete;
	AGameUI& operator=(const AGameUI& _other) = delete;
	AGameUI& operator=(AGameUI&& _other) noexcept = delete;

	void CheckInsertingCoin();
	unsigned __int8 GetCoin() const;

	static void StartTimer()
	{
		IsStop = false;
	}
	static void StopTimer()
	{
		IsStop = true;
	}
	static void ResetTimer()
	{
		Seconds = START_SECONDS;
	}
	static int GetLastTime()
	{
		return Seconds;
	}
	static bool IsTimeOver()
	{
		return Seconds < 0;
	}
	static bool IsHalfTime()
	{
		return Seconds == 60;
	}

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void LevelChangeStart() override;

private:
	void InitLowerUI();
	void InitUpperUI();

	void UpdateCoinUI();
	void UpdateScoreUI();
	void UpdatePlayer1LifeUI();
	void UpdateTimerUI(int _seconds);
	void CountDown(float _deltaTime);

	void UpdatePushStartBlink();

	StatusTopBarUI StatusTopBar;
	USpriteRenderer* SRLevel_4 = nullptr;
	USpriteRenderer* SRCredits = nullptr;
	USpriteRenderer* SRCreditCount[2] = { nullptr, nullptr };

	const char* SPRITE_TIME_COUNT = "Title_countdown_24x24.png";
	int PrevCoin = -1;
	int PrevScore = -1;
	int PrevP1Life = -1;

	static bool IsStop;
	static int Seconds;
	static const int START_SECONDS;

	float ElapsedSecs = 0.f;
	bool IsBrightPushStart = false;
};
