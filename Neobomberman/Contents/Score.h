#pragma once
#include <EngineCore/Actor.h>

class USpriteRenderer;
class AScore : public AActor
{
public:
	AScore();
	~AScore();

	AScore(const AScore& _other) = delete;
	AScore(AScore&& _other) noexcept = delete;
	AScore& operator=(const AScore& _other) = delete;
	AScore& operator=(AScore&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

private:
	void ResetTimer();
	void Countdown();

	const int START_SECONDS = 60 + 59;
	int Seconds = 0;

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
