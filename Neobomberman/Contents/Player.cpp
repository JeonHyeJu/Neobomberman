#include "PreCompile.h"
#include "Player.h"
#include <EngineCore/EngineAPICore.h>
#include <EnginePlatform/EngineInput.h>
#include "Bullet.h"

APlayer::APlayer()
{
	// UEngineAPICore::GetCore()->CreateLevel("Title");
	SetActorLocation({100, 100});
	SetActorScale({ 256, 256 });

	SetSprite("Player_Right.png");
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
		SetSprite("Player_Right.png", MySpriteIndex);
		++MySpriteIndex;
	}
}