#include "PreCompile.h"
#include "Player.h"
#include <EngineCore/EngineAPICore.h>

APlayer::APlayer()
: AActor()
{
	SetActorLoaction({100, 100});
	SetActorScale({ 100, 100 });
}

APlayer::~APlayer()
{

}

void APlayer::BeginPlay()
{
}

void APlayer::Tick()
{
	AddActorLoaction(FVector2D::RIGHT * 0.0001f);
}