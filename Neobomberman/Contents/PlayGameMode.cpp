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

	// Temp. to test collision
	AMushroom* monsterMushroom = pLevel->SpawnActor<AMushroom>();
	monsterMushroom->SetCurMap(pStage1);
	monsterMushroom->SetFirstDestination({ 0, 0 });
	monsterMushroom->SetActorLocation(Player->GetActorLocation() + FVector2D({ 64, 0 }) - FVector2D({ 16, 16 }));
	
	// TODO: spawn delay
	/*{
		AMushroom* monsterMushroom = pLevel->SpawnActor<AMushroom>();
		monsterMushroom->SetCurMap(pStage1);
		monsterMushroom->SetFirstDestination({ 5, 10 });
		monsterMushroom->SetActorLocation(monsterStartingLoc);
	}

	{
		AMushroom* monsterMushroom = pLevel->SpawnActor<AMushroom>();
		monsterMushroom->SetCurMap(pStage1);
		monsterMushroom->SetFirstDestination({ 6, 9 });
		monsterMushroom->SetActorLocation(monsterStartingLoc);
		monsterMushroom->SetStartDelay(1.f);
	}

	{
		ABalloon* monsterBalloon = pLevel->SpawnActor<ABalloon>();
		monsterBalloon->SetCurMap(pStage1);
		monsterBalloon->SetFirstDestination({ 6, 9 });
		monsterBalloon->SetActorLocation(monsterStartingLoc);
		monsterBalloon->SetStartDelay(2.f);
	}

	{
		ABalloon* monsterBalloon = pLevel->SpawnActor<ABalloon>();
		monsterBalloon->SetCurMap(pStage1);
		monsterBalloon->SetFirstDestination({ 7, 10 });
		monsterBalloon->SetActorLocation(monsterStartingLoc);
		monsterBalloon->SetStartDelay(3.f);
	}*/
	
	//pStage2->SetPortalIdx(FIntPoint(6, 10));	// temp
}

