#include "PreCompile.h"
#include "TitleGameMode.h"
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
	AFade* fade = GetWorld()->SpawnActor<AFade>();
	ATitle* logo = GetWorld()->SpawnActor<ATitle>();

	fade->FadeIn();
}