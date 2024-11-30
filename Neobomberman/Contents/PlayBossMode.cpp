#include "PreCompile.h"
#include "PlayBossMode.h"
#include "GameUI.h"
#include "BossMap.h"
#include "HoopGhost.h"
#include "Monster.h"
#include "Player.h"
#include "GameData.h"
#include "StageTitle.h"
#include "Fade.h"

#include <EngineCore/Level.h>
#include <EnginePlatform/EngineInput.h>
#include <EnginePlatform/EngineSound.h>

APlayBossMode::APlayBossMode()
{
	InitResultScene("Ending", "ResultImage2.png");
}

APlayBossMode::~APlayBossMode()
{
}

void APlayBossMode::BeginPlay()
{
	ABaseGameMode::BeginPlay();

	ULevel* pLevel = GetWorld();

	GameUiPtr = pLevel->SpawnActor<AGameUI>();

	Player = pLevel->GetPawn<APlayer>();
	Player->SetGameUI(GameUiPtr);

	/* Stage 1-1 */
	ABossMap* pBossMap = pLevel->SpawnActor<ABossMap>();
	pBossMap->Initialize();
	pBossMap->BindExplodeEvent(std::bind(&APlayBossMode::OnExplodeBomb, this));
	MapPtr = pBossMap;

	Player->SetCurMap(pBossMap);
	Player->SetStartLoc(pBossMap->MatrixIdxToLocation(StartPoint));

	SpawnMonster<AHoopGhost>(pBossMap, { 256, 126 }, 3.f);

	AStageTitle* stageTitle = pLevel->SpawnActor<AStageTitle>();
	stageTitle->SetSubStage(2);

	AFade* fade = GetWorld()->SpawnActor<AFade>();
	fade->FadeIn();

	GameUiPtr->StartTimer();

	UEngineSound::Play(SFXBg, -1, 100);
}

void APlayBossMode::Tick(float _deltaTime)
{
	ABaseGameMode::Tick(_deltaTime);
}

void APlayBossMode::FinishGame()
{
	AGameUI::StopTimer();

	if (!IsShowingResult)
	{
		IsShowingResult = true;
		Player->ShowWinnerPose();
		ShowResult();
	}
}

//void APlayBossMode::OnExplodeBombBoss()
//{
//	if (MapPtr == nullptr) return;
//
//	const std::vector<FIntPoint>& vec = MapPtr->GetSplashTileIdxs();
//
//	std::list<AMonster*>::iterator it = MonsterList.begin();
//	std::list<AMonster*>::iterator itEnd = MonsterList.end();
//	for (; it != itEnd; ++it)
//	{
//		AMonster* monster = *it;
//		if (monster->GetCanHit())
//		{
//			FIntPoint rangeIdx = monster->GetDamageRange();
//
//			if (rangeIdx.X == 1 && rangeIdx.Y == 1)
//			{
//				FIntPoint curIdx = MapPtr->LocationToMatrixIdx(monster->GetActorLocation());
//				if (MapPtr->IsInSplash(curIdx))
//				{
//					monster->Damaged();
//				}
//				return;
//			}
//
//			// I had to subtract from left top base.
//			FVector2D monsterSize = monster->GetMonsterSize().Half().Half();
//			FVector2D monsterLoc =monster->GetActorLocation();
//
//			FIntPoint orgIdx = MapPtr->LocationToMatrixIdx(monster->GetActorLocation());
//			FIntPoint curIdx = MapPtr->LocationToMatrixIdx(monster->GetActorLocation() + monsterSize);
//
//			/*OutputDebugString(("monsterLoc: " + std::to_string(monsterLoc.X) + ", " + std::to_string(monsterLoc.Y) + "\n").c_str());
//			OutputDebugString(("orgIdx: " + std::to_string(orgIdx.X) + ", " + std::to_string(orgIdx.Y) + "\n").c_str());
//			OutputDebugString(("curIdx: " + std::to_string(curIdx.X) + ", " + std::to_string(curIdx.Y) + "\n").c_str());*/
//
//			std::vector<FIntPoint> temp;	// TODO: check duplicated value
//			for (int y = -rangeIdx.Y + 1; y < rangeIdx.Y; ++y)
//			{
//				for (int x = -rangeIdx.X + 1; x < rangeIdx.X; ++x)
//				{
//					temp.push_back(curIdx + FIntPoint( x, y ));
//				}
//			}
//
//			//OutputDebugString("---------------------------- S\n");
//			for (size_t i = 0, size = temp.size(); i < size; ++i)
//			{
//				//OutputDebugString(("checkIdx: " + std::to_string(temp[i].X) + ", " + std::to_string(temp[i].Y) + "\n").c_str());
//				if (MapPtr->IsInSplash(temp[i]))
//				{
//					monster->Damaged();
//				}
//			}
//			
//			//OutputDebugString("---------------------------- E\n");
//		}
//	}
//
//	// Check player
//	FIntPoint playerIdx = MapPtr->LocationToMatrixIdx(Player->GetActorLocation());
//	if (MapPtr->IsInSplash(playerIdx))
//	{
//		Player->Kill();
//	}
//}