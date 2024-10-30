#include "PreCompile.h"
#include "Player.h"
#include "Bullet.h"

#include <EngineCore/EngineAPICore.h>
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/ImageManager.h>
#include "GlobalVar.h"

APlayer::APlayer()
{
	SetActorLocation({100, 100});
	SetActorScale(GlobalVar::BOMBERMAN_SIZE);
	
	SetSprite(PLAYER_SPRITE_PATH);
	UImageManager::GetInst().CuttingSprite(PLAYER_SPRITE_PATH, GlobalVar::BOMBERMAN_SIZE);
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	Super::BeginPlay();
}

void APlayer::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (true == UEngineInput::GetInst().IsPress('D'))
	{
		AddActorLocation(FVector2D::RIGHT * _DeltaTime * Speed);
	}
	if (true == UEngineInput::GetInst().IsPress('A'))
	{
		AddActorLocation(FVector2D::LEFT * _DeltaTime * Speed);
	}
	if (true == UEngineInput::GetInst().IsPress('S'))
	{
		AddActorLocation(FVector2D::DOWN * _DeltaTime * Speed);
	}
	if (true == UEngineInput::GetInst().IsPress('W'))
	{
		AddActorLocation(FVector2D::UP * _DeltaTime * Speed);
	}
	if (true == UEngineInput::GetInst().IsDown('R'))
	{
		SetSprite(PLAYER_SPRITE_PATH, CurIndex++);
		CurIndex %= 6;
	}
}