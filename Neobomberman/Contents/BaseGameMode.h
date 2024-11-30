#pragma once
#include <EngineCore/GameMode.h>
#include <EngineBase/FSMStateManager.h>

class ABaseGameMode : public AGameMode
{
public:
	enum class ESceneState
	{
		NONE = 0,
		RUNNING_GAME,
		FINISHING_GAME,
		SHOWING_RESULT,
		SHOWING_CONTINUE,
		GAMEOVER,
	};

	ABaseGameMode();
	~ABaseGameMode();

	ABaseGameMode(const ABaseGameMode& _other) = delete;
	ABaseGameMode(ABaseGameMode&& _other) noexcept = delete;
	ABaseGameMode& operator=(const ABaseGameMode& _other) = delete;
	ABaseGameMode& operator=(ABaseGameMode&& _other) noexcept = delete;

	template <typename MonsterType>
	MonsterType* SpawnMonster(class ABaseMap* _mapPtr, const FVector2D& _curLoc, float _startDelay=0.f, const FIntPoint& _firstHeadingIdx=FIntPoint::NEGATIVE_ONE)
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
		return monster;
	}

	void ShowResult();
	virtual void OnFinishGame() {};
	virtual void FinishingGame(float _deltaTime) {};

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;

	/* Fsm start callbacks */
	void OnShowResult();
	void OnShowContinue();
	
	/* Fsm update callbacks */
	void RunningGame(float _deltaTime);
	void ShowingContinue(float _deltaTime);

	void InitResultScene(std::string_view _nextLevel, std::string_view _rImage);
	void InitBgMusic(std::string_view _soundName)
	{
		SFXBg = _soundName;
	}

	bool IsAllMonstersDead() const;
	void CheckTimeOver();
	void CheckDeadMonster();

	void RevivalFromCoin();

	void StopActorsAndMusic();
	void RestartActorsAndMusic();

	void _ShowResultScene();
	void _GoToTitle();

	void OnHalfTime();
	void OnExplodeBomb();

	bool IsInExplosion4Edges(const FVector2D& _loc, const FVector2D& _bodySize, const URect& _margin);
	void CheckAfterExplosion(float _deltaTime);	// TODO
	std::vector<FIntPoint> SplashTileIdxsBackup;
	bool IsSplashCheck = false;

	void PlayBgMusic();

	/* Cheat */
	void DebugExlosionEdge();
	void CheckCheat();

	class ABaseMap* MapPtr = nullptr;
	class APlayer* Player = nullptr;
	class AGameUI* GameUIPtr = nullptr;

	std::list<class AMonster*> MonsterList;

	std::string NextLevelName = "";
	std::string RImageName = "";

	float ElapsedSecs = 0.f;
	bool IsOverHalfTime = false;

	/* Sounds */
	std::string SFXBg = "";
	const char* SFXCoin = "Coin.mp3";
	const char* SFXBgHurryUp = "HurryUpMusic.mp3";
	const char* SFXAlertHurryUp = "HurryUpWarning.mp3";

	int BGPositionWhenLoopStart = 0;

private:
	class AGameOver* GameOverScenePtr = nullptr;

	UFSMStateManager Fsm;
};
