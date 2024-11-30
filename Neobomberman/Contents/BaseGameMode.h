#pragma once
#include <EngineCore/GameMode.h>

class ABaseGameMode : public AGameMode
{
public:
	ABaseGameMode();
	~ABaseGameMode();

	ABaseGameMode(const ABaseGameMode& _other) = delete;
	ABaseGameMode(ABaseGameMode&& _other) noexcept = delete;
	ABaseGameMode& operator=(const ABaseGameMode& _other) = delete;
	ABaseGameMode& operator=(ABaseGameMode&& _other) noexcept = delete;

	template <typename MonsterType>
	void SpawnMonster(class ABaseMap* _mapPtr, const FVector2D& _curLoc, float _startDelay=0.f, const FIntPoint& _firstHeadingIdx=FIntPoint::NEGATIVE_ONE)
	{
		MonsterType* monster = GetWorld()->SpawnActor<MonsterType>();
		monster->SetCurMap(_mapPtr);
		monster->SetActorLocation(_curLoc);
		monster->SetStartDelay(_startDelay);
		if (_firstHeadingIdx != FIntPoint::NEGATIVE_ONE)
		{
			monster->SetFirstDestination(_firstHeadingIdx);
		}
		MonsterList.push_back(monster);
	}

	void ShowResult();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;
	virtual void LevelChangeStart() override;
	virtual void FinishGame() = 0;

	void InitResultScene(std::string_view _nextLevel, std::string_view _rImage);
	bool IsAllMonstersDead() const;
	void CheckTimeOver();
	void CheckDeadMonster();
	void CheckGameOver();

	void RevivalFromCoin();
	void GameOver();

	void StopGame();
	void RestartGame();

	void _ShowResultScene();
	void _GoToTitle();

	void OnExplodeBomb();

	bool IsInExplosion4Edges(const FVector2D& _loc, const FVector2D& _bodySize, const URect& _margin);
	void CheckAfterExplosion(float _deltaTime);	// TODO
	std::vector<FIntPoint> SplashTileIdxsBackup;
	bool IsSplashCheck = false;

	/* Cheat */
	void DebugExlosionEdge();
	void CheckCheat();

	class ABaseMap* MapPtr = nullptr;
	class APlayer* Player = nullptr;

	std::list<class AMonster*> MonsterList;

	FIntPoint StartPoint = { 0, 0 };

	bool IsShowContinueScene = false;
	bool IsShowingResult = false;

	std::string NextLevelName = "";
	std::string RImageName = "";

	float ElapsedSecs = 0.f;

	/* Sounds */
	const char* SFXCoin = "Coin.mp3";

private:
	class AGameOver* GameOverScenePtr = nullptr;
};
