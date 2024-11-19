#pragma once
#include <EngineCore/Actor.h>

class USpriteRenderer;
class AGameTimer : public AActor
{
public:
	AGameTimer();
	~AGameTimer();

	AGameTimer(const AGameTimer& _other) = delete;
	AGameTimer(AGameTimer&& _other) noexcept = delete;
	AGameTimer& operator=(const AGameTimer& _other) = delete;
	AGameTimer& operator=(AGameTimer&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	static void Start()
	{
		IsStop = false;
	}
	static void Stop()
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

private:
	void Countdown();

	static bool IsStop;
	static int Seconds;
	static const int START_SECONDS;

	const char* SCORE_BAR_ADV_SPRITE_PATH = "ScoreBar_Adventure.png";
	const char* SCORE_PUSH_START_PATH = "PushStart.png";
	const char* SCORE_PUSH_START_BR_PATH = "PushStart_Bright.png";
	const char* SCORE_TIME_COUNT = "TimeCount.png";

	USpriteRenderer* SRBar = nullptr;
	USpriteRenderer* SRPushStart = nullptr;
	USpriteRenderer* SRPushStartBr = nullptr;

	std::vector<USpriteRenderer*> SRTimerCounts;

	USpriteRenderer* SRTimerCount_1 = nullptr;		// [1] : 59
	USpriteRenderer* SRTimerCount_Col = nullptr;	// 1 [:] 59
	USpriteRenderer* SRTimerCount_2 = nullptr;		// 1 : [5]9
	USpriteRenderer* SRTimerCount_3 = nullptr;		// // 1 : 5[9]
};
