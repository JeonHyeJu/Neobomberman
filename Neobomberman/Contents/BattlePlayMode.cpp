#include "PreCompile.h"
#include "BattlePlayMode.h"
#include "BattlePlay.h"
#include "Fade.h"

ABattlePlayMode::ABattlePlayMode()
{

}

ABattlePlayMode::~ABattlePlayMode()
{

}

void ABattlePlayMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	ABattlePlay* battlePlay = pLevel->SpawnActor<ABattlePlay>();

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();
}