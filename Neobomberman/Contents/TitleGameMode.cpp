#include "PreCompile.h"
#include "TitleGameMode.h"
#include "TitleLogo.h"

ATitleGameMode::ATitleGameMode()
{
}

ATitleGameMode::~ATitleGameMode()
{
}

void ATitleGameMode::BeginPlay()
{
	Super::BeginPlay();
	TitleLogo* logo = GetWorld()->SpawnActor<TitleLogo>();
}