#include "PreCompile.h"
#include "BattleSelectMode.h"
#include "BattleSelect.h"
#include "Fade.h"

ABattleSelectMode::ABattleSelectMode()
{

}

ABattleSelectMode::~ABattleSelectMode()
{

}

void ABattleSelectMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	ABattleSelect* selectMapNChar = pLevel->SpawnActor<ABattleSelect>();

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();
}