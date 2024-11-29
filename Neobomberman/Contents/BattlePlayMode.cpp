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

	AGameUI* gameUI = pLevel->SpawnActor<AGameUI>();

	std::vector<EItem> itemList = { EItem::BOMB, EItem::BOMB, EItem::SPEED, EItem::SPEED, EItem::POWER, EItem::POWER };

	ABattleMap* mapPtr = pLevel->SpawnActor<ABattleMap>();
	mapPtr->InitMap();
	mapPtr->BindExplodeEvent(std::bind(&ABattlePlayMode::OnExplodeBomb, this));
	mapPtr->SetItems(itemList);	// Must set after InitMap
	CurMapPtr = mapPtr;

	Player = pLevel->GetPawn<APlayer>();
	Player->SetGameUI(gameUI);
	Player->SetCurMap(mapPtr);
	Player->SetStartLoc(mapPtr->MatrixIdxToLocation(StartPoint));

	PlayerComputer = pLevel->SpawnActor<APlayerComputer>();
	PlayerComputer->InitSprite("MainCharater_Black.png");		// Temp
	PlayerComputer->SetStartLoc(mapPtr->MatrixIdxToLocation(StartPointComputer));

	AStageTitle* stageTitle = pLevel->SpawnActor<AStageTitle>();
	stageTitle->SetSubStage(1);

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();

	gameUI->StartTimer();

	UEngineSound::Play(SFXBg, -1, 100);
}

void ABattlePlayMode::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	static float elapsedSecs = 0.f;
	elapsedSecs += _deltaTime;

	if (elapsedSecs > 2.f)
	{
		elapsedSecs = 0.f;

		if (!IsStarted)
		{
			IsStarted = true;
			UEngineSound::Play(SFXReadyGo);
		}
	}
}

void ABattlePlayMode::OnExplodeBomb()
{
	if (CurMapPtr == nullptr) return;

	SplashTileIdxsBackup = CurMapPtr->GetSplashTileIdxs();
	IsSplashCheck = true;

	CheckAfterExplosion(.4f);
}

// Hack code.. lasting explosion impact..
void ABattlePlayMode::CheckAfterExplosion(float _deltaTime)
{
	if (CurMapPtr == nullptr) return;
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

		// Check player
		FVector2D playerLoc = Player->GetActorLocation();
		FVector2D margin{ 10, 10 };

		FVector2D nextPosLT = playerLoc + margin;
		FVector2D nextPosRT = FVector2D{ nextPosLT.X + GlobalVar::BOMBERMAN_SIZE.hX() * .5f, nextPosLT.Y };
		FVector2D nextPosLB = FVector2D{ nextPosLT.X, playerLoc.Y + GlobalVar::BOMBERMAN_SIZE.hY() * .4f + margin.Y };
		FVector2D nextPosRB = FVector2D{ nextPosRT.X, nextPosLB.Y };

		FIntPoint nextIdxLT = CurMapPtr->LocationToMatrixIdx(nextPosLT);
		FIntPoint nextIdxRT = CurMapPtr->LocationToMatrixIdx(nextPosRT);
		FIntPoint nextIdxLB = CurMapPtr->LocationToMatrixIdx(nextPosLB);
		FIntPoint nextIdxRB = CurMapPtr->LocationToMatrixIdx(nextPosRB);

		bool isInSplashLT = CurMapPtr->IsInSplashWithVector(nextIdxLT, SplashTileIdxsBackup);
		bool isInSplashRT = CurMapPtr->IsInSplashWithVector(nextIdxRT, SplashTileIdxsBackup);
		bool isInSplashLB = CurMapPtr->IsInSplashWithVector(nextIdxLB, SplashTileIdxsBackup);
		bool isInSplashRB = CurMapPtr->IsInSplashWithVector(nextIdxRB, SplashTileIdxsBackup);

		/*OutputDebugString(("nextIdxLT : " + std::to_string(nextIdxLT.X) + ", " + std::to_string(nextIdxLT.Y) + "\n").c_str());
		OutputDebugString(("nextIdxRT : " + std::to_string(nextIdxRT.X) + ", " + std::to_string(nextIdxRT.Y) + "\n").c_str());
		OutputDebugString(("nextIdxLB : " + std::to_string(nextIdxLB.X) + ", " + std::to_string(nextIdxLB.Y) + "\n").c_str());
		OutputDebugString(("nextIdxRB : " + std::to_string(nextIdxRB.X) + ", " + std::to_string(nextIdxRB.Y) + "\n").c_str());
		OutputDebugString("----------------------------------\n");*/
		if (isInSplashLT || isInSplashRT || isInSplashLB || isInSplashRB)
		{
			Player->Kill();
		}

		// Check item
		for (size_t i = 0, size = SplashTileIdxsBackup.size(); i < size; ++i)
		{
			FIntPoint pt = SplashTileIdxsBackup[i];
			if (CurMapPtr->HasShowingItem(pt))
			{
				EItem item = CurMapPtr->PopItem(pt);	// Remove
			}
		}

		executeCnt++;
	}
}

void ABattlePlayMode::OnGameFinished()
{
	static bool temp = true;

	if (temp)
	{
		temp = false;
		UEngineSound::Play(SFXRing);
		UEngineSound::StopPlayer(SFXBg);
	}
}