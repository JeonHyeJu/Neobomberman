#include "PreCompile.h"
#include "PlayBossMode.h"
#include "GameUI.h"
#include "BossMap.h"
#include "HoopGhost.h"
#include "Monster.h"
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

	AHoopGhost* boss = pLevel->SpawnActor<AHoopGhost>();
	boss->SetCurMap(pBossMap);
	boss->SetFirstDestination({ 0, 0 });
	boss->SetActorLocation({ 256, 256 });	// Temp
	MonsterList.push_back(boss);

	AFade* fade = GetWorld()->SpawnActor<AFade>();
	fade->FadeIn();

	gameUI->StartTimer();
}

void APlayBossMode::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}