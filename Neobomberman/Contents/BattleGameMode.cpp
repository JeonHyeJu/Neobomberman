#include "PreCompile.h"
#include "BattleGameMode.h"
#include "SelectMap.h"

ABattleGameMode::ABattleGameMode()
{

}

ABattleGameMode::~ABattleGameMode()
{

}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	ASelectMap* selectMap = pLevel->SpawnActor<ASelectMap>();
}