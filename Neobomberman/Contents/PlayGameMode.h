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

protected:

private:
	void CheckGameOver();
	void CheckDeadMonster();
	bool IsAllMonstersDead() const;
	void OnExplodeBomb();
	void GameOver();

	void OnEndGameOverFadeOut();

	class ABaseMap* CurMapPtr = nullptr;
	class AGameOver* GameOverScenePtr = nullptr;
	class AGameUI* GameUiPtr = nullptr;

	std::list<class AMonster*> MonsterList;
	const int MONSTER_CNT_STAGE_1 = 4;

	const FIntPoint PORTAL_IDX_STAGE_1 = { 6, 10 };
	bool isShowContinueScene = false;

	float ElapsedSecs = 0.f;
};

