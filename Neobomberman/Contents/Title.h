#pragma once
#include <EngineCore/Actor.h>
#include <EngineBase/FSMStateManager.h>

enum class ESceneType
{
	START = 0,
	SELECT,
};

enum class ETitleState
{
	OPENING = 0,
	WAIT_START,
	WAIT_SELECT_IDLE,
	WAIT_SELECT,
	RUN_CUT_SCENE_IDLE,
	RUN_CUT_SCENE,
	PREPARE_PLAY,
	PREPARE_DISAPPEAR
};

class USpriteRenderer;
class ATitle : public AActor
{
public:
	ATitle();
	~ATitle();

	ATitle(const ATitle& _Other) = delete;
	ATitle(ATitle&& _Other) noexcept = delete;
	ATitle& operator=(const ATitle& _Other) = delete;
	ATitle& operator=(ATitle&& _Other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void LevelChangeStart();

private:
	void ResetSeconds();
	void AddCoin(int _coinCnt);
	void SwitchStartUi(bool _isShow);
	void SwitchSelectUi(bool _isShow);
	void SwitchCutSceneUi(bool _isShow);

	void RunWaitSequence(float _deltaTime, const ESceneType& _type);
	void RunPaintSequence(float _deltaTime);
	void Countdown(const ESceneType& _type);
	void ChangeToCutScene();
	void ChangeToStartScene();

	void OnEndAnimation();
	void OnEndPainterDraw();
	void OnEndCircleDraw();
	void OnEndFadeOut();

	void OnRunOpening();
	void OnEndCutScene();

	void OnWaitToStart();
	void OnSelectMode();
	void OnRunCutScene();

	void SelectingMode(float _deltaTime);
	void WaitingToStart(float _deltaTime);
	void RunningCutScene(float _deltaTime);

	void CoinAction();

	const char* ANIM_IDLE_NAME = "Opening_Idle";
	const char* ANIM_RUN_NAME = "Opening_Run";

	const char* SPRITE_TIME_COUNT = "Title_countdown_24x24.png";
	const std::string SPRITE_SELECT_TIME_COUNT_B = "BlueCount.png";
	const std::string SPRITE_SELECT_TIME_COUNT_P = "PinkCount.png";
	const std::string SPRITE_SELECT_TIME_COUNT_Y = "YellowCount.png";

	USpriteRenderer* SROpening = nullptr;
	USpriteRenderer* SRTimeTitle = nullptr;
	USpriteRenderer* SRTimeCount[2] = { nullptr, nullptr };
	USpriteRenderer* SRLevel_4 = nullptr;
	USpriteRenderer* SRCredits = nullptr;
	USpriteRenderer* SRCreditCount[2] = { nullptr, nullptr };
	USpriteRenderer* SRPushP1P2 = nullptr;

	USpriteRenderer* SRSelectGameMode = nullptr;
	USpriteRenderer* SRSelectTimeCount[2] = { nullptr, nullptr };
	USpriteRenderer* SRSelectSayHi = nullptr;

	USpriteRenderer* SRSelectPainter = nullptr;
	USpriteRenderer* SRSelectCircle = nullptr;

	USpriteRenderer* SRCutSceneBg = nullptr;
	USpriteRenderer* SRCutScene = nullptr;
	USpriteRenderer* SRSpaceship = nullptr;
	USpriteRenderer* SRLetterBoxUpper = nullptr;
	USpriteRenderer* SRLetterBoxLower = nullptr;

	int Seconds = 0;
	unsigned __int8 Coin = 0;

	const int START_SECONDS = 29;

	const FVector2D PAINTER_START_LOC = { 64, 224 };
	const FVector2D PAINTER_END_LOC = { 64, 384 };

	bool isPainterMovingUp = false;
	bool isPainterMovingDown = false;

	UFSMStateManager FSM;
};