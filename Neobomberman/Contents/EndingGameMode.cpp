#include "PreCompile.h"
#include "EndingGameMode.h"
#include "Ending.h"
#include "Fade.h"

AEndingGameMode::AEndingGameMode()
{
}

AEndingGameMode::~AEndingGameMode()
{
}

void AEndingGameMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();
	AFade* fade = pLevel->SpawnActor<AFade>();

	AEnding* ending = pLevel->SpawnActor<AEnding>();
	fade->FadeIn();
}
