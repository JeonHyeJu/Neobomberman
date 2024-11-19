#pragma once
#include <EngineCore/Actor.h>
#include <EngineBase/FSMStateManager.h>

#define SHOW_NUMBER_CNT 6

enum class EResultState
{
	INIT = 0,
	MOVE,
	CALCULATE_BONUS,
	CALCULATE_TOTAL,
	STAMP,
	SHOW_RIMAGE,	// Right image
	SHAKE_RIMAGE,
};

class USpriteRenderer;
class AResult : public AActor
{
public:
	AResult();
	~AResult();

	AResult(const AResult& _other) = delete;
	AResult(AResult&& _other) noexcept = delete;
	AResult& operator=(const AResult& _other) = delete;
	AResult& operator=(AResult&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	inline void SetLastSecs(int _lastSecs)
	{
		LastSecs = _lastSecs;
	}
	inline void SetTotal(int _prevTotal)
	{
		Total = _prevTotal;
	}

private:
	void SetLastTimeUI(int _lastSecs);
	void SetBonusUI(int _bonus);
	void SetTotalUI(int _total);

	void HideAllBonusUI();
	void HideAllTotalUI();

	void Moving(float _deltaTime);
	void CalculatingBonus(float _deltaTime);
	void CalculatingTotal(float _deltaTime);
	void Stamping(float _deltaTime);
	void ShowingRImage(float _deltaTime);
	void ShakingRImage(float _deltaTime);

	USpriteRenderer* SRStageClear = nullptr;
	USpriteRenderer* SRResultFrame = nullptr;
	USpriteRenderer* SRResultRImage = nullptr;
	USpriteRenderer* SRStamp = nullptr;
	USpriteRenderer* SRTime[3] = { nullptr, };
	USpriteRenderer* SRBonus[SHOW_NUMBER_CNT] = { nullptr, };
	USpriteRenderer* SRTotal[SHOW_NUMBER_CNT] = { nullptr, };

	const char* RESULT_COUNT_PATH = "ResultCount.png";
	float Speed = 700.f;
	float SpeedRImage = 500.f;

	int LastSecs = 0;
	int Bonus = 0;
	int Total = 0;

	UFSMStateManager FSM;

	FVector2D LastLocRImage = FVector2D{ 460, 172 };
	float ElapsedSecs = 0.f;
};
