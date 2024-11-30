#include "PreCompile.h"
#include "BattlePlayMode.h"
#include "GlobalVar.h"
#include "BattleMap.h"
#include "Player.h"
#include "GameUI.h"
#include "StageTitle.h"
#include "PlayerComputer.h"
#include "Fade.h"

#include <EnginePlatform/EngineSound.h>
#include <EnginePlatform/EngineInput.h>

ABattlePlayMode::ABattlePlayMode()
{
}

ABattlePlayMode::~ABattlePlayMode()
{
}

void ABattlePlayMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	std::vector<EItem> itemList = { EItem::BOMB, EItem::BOMB, EItem::SPEED, EItem::SPEED, EItem::POWER, EItem::POWER };

	GameUIPtr = pLevel->SpawnActor<AGameUI>();

	ABattleMap* mapPtr = pLevel->SpawnActor<ABattleMap>();
	mapPtr->Initialize(itemList);
	mapPtr->BindExplodeEvent(std::bind(&ABattlePlayMode::OnExplodeBomb, this));
	MapPtr = mapPtr;

	Player = pLevel->GetPawn<APlayer>();
	Player->SetCurMap(mapPtr);
	Player->SetStartLoc(mapPtr->MatrixIdxToLocation(StartPoint));

	FVector2D comStartLoc = mapPtr->MatrixIdxToLocation(StartPointComputer);
	APlayerComputer* playerComputer = SpawnPlayerComputer<APlayerComputer>(mapPtr, comStartLoc);
	playerComputer->SetStartLoc(comStartLoc);
	playerComputer->InitSprite("MainCharater_Black.png");		// Temp

	AStageTitle* stageTitle = pLevel->SpawnActor<AStageTitle>();
	stageTitle->SetSubStage(1);

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();

	GameUIPtr->StartTimer();

	UEngineSound::Play(SFXBg, -1, 100);
}

void ABattlePlayMode::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	static float elapsedSecs = 0.f;
	elapsedSecs += _deltaTime;

	if (elapsedSecs > .5f)
	{
		elapsedSecs = 0.f;

		if (!IsStarted)
		{
			IsStarted = true;
			UEngineSound::Play(SFXReadyGo);
		}

		CheckDeadEnemy();

		if (IsAllEnemiesDead())
		{
			static bool temp = true;
			if (temp)
			{
				temp = false;
				OnFinishGame();
				FinishingGame(_deltaTime);
			}
		}
	}
	
	CheckCheat();
}


bool ABattlePlayMode::IsAllEnemiesDead() const
{
	return ComputerList.size() == 0;
}

void ABattlePlayMode::CheckDeadEnemy()
{
	std::list<APlayerComputer*>::iterator it = ComputerList.begin();
	std::list<APlayerComputer*>::iterator itEnd = ComputerList.end();
	for (; it != itEnd; ++it)
	{
		APlayerComputer* computer = *it;
		if (computer->GetIsDead())
		{
			it = ComputerList.erase(it);
			if (it == itEnd) break;
		}
	}
}

void ABattlePlayMode::OnExplodeBomb()
{
	if (MapPtr == nullptr) return;

	SplashTileIdxsBackup = MapPtr->GetSplashTileIdxs();
	IsSplashCheck = true;

	CheckAfterExplosion(.4f);
}

bool ABattlePlayMode::IsInExplosion4Edges(const FVector2D& _loc, const FVector2D& _bodySize, const URect& _margin)
{
	FVector2D nextPosLT = _loc + FVector2D{ _margin.Left, _margin.Top };
	FVector2D nextPosRT = _loc + FVector2D{ _bodySize.X - _margin.Right, _margin.Top };;
	FVector2D nextPosLB = _loc + FVector2D{ _margin.Left, _bodySize.Y - _margin.Bottom };
	FVector2D nextPosRB = _loc + FVector2D{ _bodySize.X - _margin.Right, _bodySize.Y - _margin.Bottom };

	FIntPoint nextIdxLT = MapPtr->LocationToMatrixIdx(nextPosLT);
	FIntPoint nextIdxRT = MapPtr->LocationToMatrixIdx(nextPosRT);
	FIntPoint nextIdxLB = MapPtr->LocationToMatrixIdx(nextPosLB);
	FIntPoint nextIdxRB = MapPtr->LocationToMatrixIdx(nextPosRB);

	int subX = nextIdxRB.X - nextIdxLT.X;
	int subY = nextIdxRB.Y - nextIdxLT.Y;
	for (int x = 0; x <= subX; ++x)
	{
		for (int y = 0; y <= subY; ++y)
		{
			FIntPoint point = { nextIdxLT.X + x, nextIdxLT.Y + y };

			bool isInSplash = MapPtr->IsInSplashWithVector(point, SplashTileIdxsBackup);
			//OutputDebugString(("interIdxs[" + std::to_string(x) + ", " + std::to_string(y) + "] : " + std::to_string(point.X) + ", " + std::to_string(point.Y) + "..." + (isInSplash ? "isInSplash O" : "isInSplash X") + "\n").c_str());
			if (isInSplash)
			{
				return true;
			}
		}
	}

	return false;
}

// Hack code.. lasting explosion impact..
void ABattlePlayMode::CheckAfterExplosion(float _deltaTime)
{
	if (MapPtr == nullptr) return;
	if (!IsSplashCheck) return;
	if (SplashTileIdxsBackup.size() == 0) return;

	static float elapsedSecs = 0.f;
	static int executeCnt = 0;

	elapsedSecs += _deltaTime;

	if (elapsedSecs >= .4f)	// match with explosion anim
	{
		elapsedSecs = 0.f;
		if (executeCnt > 3)
		{
			executeCnt = 0;
			IsSplashCheck = false;
			return;
		}

		// Check monsters
		std::list<APlayerComputer*>::iterator it = ComputerList.begin();
		std::list<APlayerComputer*>::iterator itEnd = ComputerList.end();
		for (; it != itEnd; ++it)
		{
			APlayerComputer* computer = *it;
			if (computer->GetCanHit())
			{
				const FVector2D& monsterLoc = computer->GetActorLocation();
				const FVector2D& damageSize = computer->GetDamageSize();
				const URect& damageMargin = computer->GetDamageMargin();

				if (IsInExplosion4Edges(monsterLoc, damageSize, damageMargin))
				{
					computer->Kill();
				}
			}
		}

		// Check player
		{
			const FVector2D& playerLoc = Player->GetActorLocation();
			const FVector2D& damageSize = Player->GetDamageSize();
			const URect& damageMargin = Player->GetDamageMargin();

			if (IsInExplosion4Edges(playerLoc, damageSize, damageMargin))
			{
				Player->Kill();
			}
		}

		// Check item
		for (size_t i = 0, size = SplashTileIdxsBackup.size(); i < size; ++i)
		{
			FIntPoint pt = SplashTileIdxsBackup[i];
			if (MapPtr->HasShowingItem(pt))
			{
				EItem item = MapPtr->PopItem(pt);	// Remove
			}
		}

		executeCnt++;
	}
}

void ABattlePlayMode::OnFinishGame()
{
	UEngineSound::Play(SFXRing);
	//UEngineSound::StopPlayer(SFXBg);
}

void ABattlePlayMode::FinishingGame(float _deltaTime)
{
}

void ABattlePlayMode::CheckCheat()
{
	if (UEngineInput::GetInst().IsDown('N'))
	{
		std::list<APlayerComputer*>::iterator it = ComputerList.begin();
		std::list<APlayerComputer*>::iterator itEnd = ComputerList.end();
		for (; it != itEnd; ++it)
		{
			APlayerComputer* computer = *it;
			computer->Kill();
		}

		if (MapPtr)
		{
			MapPtr->CheatDestoyAllBoxes();
		}
	}
}