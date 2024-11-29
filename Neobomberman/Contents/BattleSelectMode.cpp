#include "PreCompile.h"
#include "BattleSelectMode.h"
#include "BattleSelect.h"
#include "GameUI.h"
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
	AGameUI* gameUI = pLevel->SpawnActor<AGameUI>();

	ABattleSelect* selectMapNChar = pLevel->SpawnActor<ABattleSelect>();

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();
}