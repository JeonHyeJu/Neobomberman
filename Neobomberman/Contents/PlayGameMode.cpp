#include "PreCompile.h"
#include "PlayGameMode.h"

#include <EngineCore/Level.h>
#include "PlayMap.h"
#include "Player.h"

APlayGameMode::APlayGameMode()
{
}

APlayGameMode::~APlayGameMode()
{
}

void APlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayer* Player = GetWorld()->GetPawn<APlayer>();
	//Player->SetCollisionImage("Bg_1-Col.png");

	APlayMap* NewActor = GetWorld()->SpawnActor<APlayMap>();
	Player->SetCurMap(NewActor);
}

