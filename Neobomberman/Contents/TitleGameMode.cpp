#include "PreCompile.h"
#include "TitleGameMode.h"
#include "Title.h"

ATitleGameMode::ATitleGameMode()
{
}

ATitleGameMode::~ATitleGameMode()
{
}

void ATitleGameMode::BeginPlay()
{
	Super::BeginPlay();
	ATitle* logo = GetWorld()->SpawnActor<ATitle>();
}