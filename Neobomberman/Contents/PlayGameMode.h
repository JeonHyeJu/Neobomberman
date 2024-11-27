#pragma once
#include <EngineCore/GameMode.h>

class APlayGameMode : public AGameMode
{
public:
	APlayGameMode();
	~APlayGameMode();

	APlayGameMode(const APlayGameMode& _Other) = delete;
	APlayGameMode(APlayGameMode&& _Other) noexcept = delete;
	APlayGameMode& operator=(const APlayGameMode& _Other) = delete;
	APlayGameMode& operator=(APlayGameMode&& _Other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void LevelChangeStart() override;

	void StopGame();
	void RestartGame();

protected:

private:
	void FadeOut();
	void OnEndFadeOut();

	void StartFromCoin();
	void CheckGameOver();
	void CheckDeadMonster();
	void CheckTimeOver();
	bool IsAllMonstersDead() const;
	void OnExplodeBomb();
	void GameOver();

	void OnEndGameOverFadeOut();

	void CheckCheat();

	FIntPoint StartPoint = { 0, 0 };

	class ABaseMap* CurMapPtr = nullptr;
	class AGameOver* GameOverScenePtr = nullptr;
	class AGameUI* GameUiPtr = nullptr;
	class AResult* ResultScene = nullptr;
	class APlayer* Player = nullptr;

	std::list<class AMonster*> MonsterList;
	const int MONSTER_CNT_STAGE_1 = 4;

	const FIntPoint PORTAL_IDX_STAGE_1 = { 6, 10 };
	bool isShowContinueScene = false;
	bool isShowingResult = false;

	float ElapsedSecs = 0.f;
};

