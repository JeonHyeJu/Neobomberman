#include "PreCompile.h"
#include "TitleGameMode.h"
#include "GameUI.h"
#include "Title.h"
#include "Fade.h"

ATitleGameMode::ATitleGameMode()
{
}

ATitleGameMode::~ATitleGameMode()
{
}

void ATitleGameMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	AGameUI* gameUI = pLevel->SpawnActor<AGameUI>();
	AFade* fade = pLevel->SpawnActor<AFade>();
	ATitle* title = pLevel->SpawnActor<ATitle>();

	title->SetGameUI(gameUI);
	gameUI->StartTimer();
}