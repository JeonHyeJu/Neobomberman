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
	Player->SetCurMap(pStage1);

	AMushroom* monsterMushroom1 = pLevel->SpawnActor<AMushroom>();
	AMushroom* monsterMushroom2 = pLevel->SpawnActor<AMushroom>();
	ABalloon* monsterBalloon1 = pLevel->SpawnActor<ABalloon>();
	ABalloon* monsterBalloon2 = pLevel->SpawnActor<ABalloon>();

	monsterMushroom1->Init("Mushroom.png");		// temp
	monsterMushroom2->Init("Mushroom.png");		// temp
	monsterBalloon1->Init("Balloon_long.png");		// temp
	monsterBalloon2->Init("Balloon_long.png");		// temp

	monsterMushroom1->SetActorLocation(FVector2D(300, 300));	// temp
	monsterMushroom2->SetActorLocation(FVector2D(300, 400));	// temp
	monsterBalloon1->SetActorLocation(FVector2D(200, 300));	// temp
	monsterBalloon2->SetActorLocation(FVector2D(400, 400));	// temp

	//pStage2->SetPortalLoc(FIntPoint(6, 10));	// temp
}

