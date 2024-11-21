#include "PreCompile.h"
#include "PlayBossMode.h"
#include "GameUI.h"
#include "BossMap.h"
#include "Player.h"
#include "Fade.h"

#include <EngineCore/Level.h>

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

	AGameUI* gameUI = pLevel->SpawnActor<AGameUI>();

	APlayer* Player = pLevel->GetPawn<APlayer>();
	Player->SetGameUI(gameUI);
	//Player->SetCollisionImage("Bg_1-Col.png");

	/* Stage 1-1 */
	ABossMap* pBossMap = pLevel->SpawnActor<ABossMap>();
	pBossMap->InitMap();

	Player->SetCurMap(pBossMap);

	// Temp. To test portal
	//Player->SetActorLocation(monsterStartingLoc + FVector2D({ 16.f, -16.f }));	// temp

	AFade* fade = GetWorld()->SpawnActor<AFade>();
	fade->FadeIn();
	gameUI->StartTimer();
}

