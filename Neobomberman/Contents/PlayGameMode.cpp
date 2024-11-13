#include "PreCompile.h"
#include "PlayGameMode.h"

#include <EngineCore/Level.h>
#include "PlayMap.h"
#include "Player.h"
#include "Mushroom.h"
#include "Balloon.h"

APlayGameMode::APlayGameMode()
{
}

APlayGameMode::~APlayGameMode()
{
}

void APlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();
	APlayer* Player = pLevel->GetPawn<APlayer>();
	//Player->SetCollisionImage("Bg_1-Col.png");

	/* Stage 1-1 */
	APlayMap* pStage1 = pLevel->SpawnActor<APlayMap>();
	pStage1->InitMap();
	Player->SetCurMap(pStage1);

	FVector2D monsterStartingLoc = pStage1->GetPortalLoc();

	AMushroom* monsterMushroom1 = pLevel->SpawnActor<AMushroom>();
	AMushroom* monsterMushroom2 = pLevel->SpawnActor<AMushroom>();

	//ABalloon* monsterBalloon1 = pLevel->SpawnActor<ABalloon>();
	//ABalloon* monsterBalloon2 = pLevel->SpawnActor<ABalloon>();

	monsterMushroom1->SetCurMap(pStage1);
	monsterMushroom2->SetCurMap(pStage1);

	//monsterBalloon1->SetCurMap(pStage1);
	//monsterBalloon2->SetCurMap(pStage1);

	monsterMushroom1->SetFirstIndex({ 5, 10 });
	monsterMushroom2->SetFirstIndex({ 6, 9 });

	monsterMushroom1->SetActorLocation(monsterStartingLoc);
	monsterMushroom2->SetActorLocation(monsterStartingLoc);

	//pStage2->SetPortalIdx(FIntPoint(6, 10));	// temp
}

