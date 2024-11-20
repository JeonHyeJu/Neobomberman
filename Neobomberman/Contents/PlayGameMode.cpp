#include "PreCompile.h"
#include "PlayGameMode.h"
#include "GameTimer.h"
#include "GameUI.h"
#include "PlayMap.h"
#include "Player.h"
#include "Monster.h"
#include "Mushroom.h"
#include "Balloon.h"
#include "Fade.h"

#include <EngineCore/Level.h>

APlayGameMode::APlayGameMode()
{
	MonsterList.reserve(MONSTER_CNT_STAGE_1);
}

APlayGameMode::~APlayGameMode()
{
}

void APlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	AGameUI* gameUI = pLevel->SpawnActor<AGameUI>();
	AGameTimer* gameTimer = pLevel->SpawnActor<AGameTimer>();

	APlayer* Player = pLevel->GetPawn<APlayer>();
	Player->SetGameUI(gameUI);
	//Player->SetCollisionImage("Bg_1-Col.png");

	/* Stage 1-1 */
	APlayMap* pStage1 = pLevel->SpawnActor<APlayMap>();
	pStage1->SetPortalIdx(PORTAL_IDX_STAGE_1);
	CurMapPtr = pStage1;

	pStage1->InitMap();
	Player->SetCurMap(pStage1);

	FVector2D monsterStartingLoc = pStage1->GetPortalLoc();

	// Temp. To test portal
	Player->SetActorLocation(monsterStartingLoc + FVector2D({ 16.f, -16.f }));	// temp

	// Temp. To test collision
	/*AMushroom* monsterMushroom = pLevel->SpawnActor<AMushroom>();
	monsterMushroom->SetCurMap(pStage1);
	monsterMushroom->SetFirstDestination({ 0, 0 });
	monsterMushroom->SetActorLocation(Player->GetActorLocation() + FVector2D({ 64, 0 }) - FVector2D({ 16, 16 }));*/
	
	// TODO: spawn delay
	/*{
		AMushroom* monster = pLevel->SpawnActor<AMushroom>();
		monster->SetCurMap(pStage1);
		monster->SetFirstDestination({ 5, 10 });
		monster->SetActorLocation(monsterStartingLoc);
		MonsterList.push_back(monster);
	}

	{
		AMushroom* monster = pLevel->SpawnActor<AMushroom>();
		monster->SetCurMap(pStage1);
		monster->SetFirstDestination({ 6, 9 });
		monster->SetActorLocation(monsterStartingLoc);
		monster->SetStartDelay(1.f);
		MonsterList.push_back(monster);
	}

	{
		ABalloon* monster = pLevel->SpawnActor<ABalloon>();
		monster->SetCurMap(pStage1);
		monster->SetFirstDestination({ 6, 9 });
		monster->SetActorLocation(monsterStartingLoc);
		monster->SetStartDelay(2.f);
		MonsterList.push_back(monster);
	}

	{
		ABalloon* monster = pLevel->SpawnActor<ABalloon>();
		monster->SetCurMap(pStage1);
		monster->SetFirstDestination({ 7, 10 });
		monster->SetActorLocation(monsterStartingLoc);
		monster->SetStartDelay(3.f);
		MonsterList.push_back(monster);
	}*/
	
	//pStage2->SetPortalIdx(FIntPoint(6, 10));	// temp

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();

	Player->InitFadeEvent(fade);
	gameTimer->Start();
}

void APlayGameMode::Tick(float _deltaTime)
{
	static float elapsedSecs = 0.f;

	elapsedSecs += _deltaTime;
	if (elapsedSecs > 1.f)
	{
		elapsedSecs = 0.f;
		bool isAllDead = true;
		for (size_t i = 0, size = MonsterList.size(); i < size; i++)
		{
			if (!MonsterList[i]->IsDestroy())
			{
				isAllDead = false;
				break;
			}
		}

		if (isAllDead)
		{
			GoNextStage();
		}
	}
}

void APlayGameMode::GoNextStage()
{
	CurMapPtr->OpenPortal();
}