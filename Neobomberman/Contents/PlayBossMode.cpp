#include "PreCompile.h"
#include "PlayBossMode.h"
#include "Fade.h"

#include <EngineCore/Level.h>

#include "Score.h"
#include "BossMap.h"
#include "Player.h"

APlayBossMode::APlayBossMode()
{
}

APlayBossMode::~APlayBossMode()
{
}

void APlayBossMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	AScore* Score = pLevel->SpawnActor<AScore>();

	APlayer* Player = pLevel->GetPawn<APlayer>();
	//Player->SetCollisionImage("Bg_1-Col.png");

	/* Stage 1-1 */
	ABossMap* pBossMap = pLevel->SpawnActor<ABossMap>();
	pBossMap->InitMap();
	//Player->SetCurMap(pBossMap);

	// Temp. To test portal
	//Player->SetActorLocation(monsterStartingLoc + FVector2D({ 16.f, -16.f }));	// temp

	//AFade* fade = GetWorld()->SpawnActor<AFade>();
	//fade->FadeIn();
}

