#pragma once
#include <EngineCore/GameMode.h>
#include <EngineBase/FSMStateManager.h>

class ABattlePlayMode : public AGameMode
{
public:
	enum class ESceneState
	{
		NONE = 0,
		RUNNING_GAME,
		FINISHING_GAME,
		SHOWING_SCOREBOARD,
		END
	};


	ABattlePlayMode();
	~ABattlePlayMode();

	ABattlePlayMode(const ABattlePlayMode& _other) = delete;
	ABattlePlayMode(ABattlePlayMode&& _other) noexcept = delete;
	ABattlePlayMode& operator=(const ABattlePlayMode& _other) = delete;
	ABattlePlayMode& operator=(ABattlePlayMode&& _other) noexcept = delete;

	template <typename ComputerType>
	ComputerType* SpawnPlayerComputer(class ABaseMap* _mapPtr, const FVector2D& _curLoc, float _startDelay = 0.f, const FIntPoint& _firstHeadingIdx = FIntPoint::NEGATIVE_ONE)
	{
		ComputerType* computer = GetWorld()->SpawnActor<ComputerType>();
		computer->SetCurMap(_mapPtr);
		computer->SetActorLocation(_curLoc);
		computer->SetStartDelay(_startDelay);
		if (_firstHeadingIdx != FIntPoint::NEGATIVE_ONE)
		{
			computer->SetFirstDestination(_firstHeadingIdx);
		}
		ComputerList.push_back(computer);
		return computer;
	}

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;
	void OnFinishGame();
	void FinishingGame(float _deltaTime);

private:
	void OnExplodeBomb();
	bool IsInExplosion4Edges(const FVector2D& _loc, const FVector2D& _bodySize, const URect& _margin);
	void CheckAfterExplosion(float _deltaTime);
	void CheckCheat();
	bool IsAllEnemiesDead() const;
	void CheckDeadEnemy();

	class APlayer* Player = nullptr;
	class ABaseMap* MapPtr = nullptr;
	class AGameUI* GameUIPtr = nullptr;

	FIntPoint StartPoint = { 0, 0 };
	FIntPoint StartPointComputer = { 12, 0 };

	bool IsStarted = false;

	std::list<class APlayerComputer*> ComputerList;

	std::vector<FIntPoint> SplashTileIdxsBackup;
	bool IsSplashCheck = false;

	/* Sounds */
	const char* SFXBg = "BattleMusic.mp3";
	const char* SFXReadyGo = "ReadyGo.mp3";
	const char* SFXRing = "Ring.mp3";

	UFSMStateManager Fsm;
};
