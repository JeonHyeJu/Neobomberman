#include "PreCompile.h"
#include "Player.h"
#include <EngineCore/EngineAPICore.h>
#include <EnginePlatform/EngineInput.h>
#include "Bullet.h"

APlayer::APlayer()
{
	SetActorLocation({100, 100});
	SetActorScale({ 100, 100 });
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	UEngineInput::GetInst().BindAction('A', KeyEvent::Press, std::bind(&APlayer::LeftMove, this, std::placeholders::_1));
	UEngineInput::GetInst().BindAction('D', KeyEvent::Press, std::bind(&APlayer::RightMove, this, std::placeholders::_1));
	UEngineInput::GetInst().BindAction('S', KeyEvent::Press, std::bind(&APlayer::DownMove, this, std::placeholders::_1));
	UEngineInput::GetInst().BindAction('W', KeyEvent::Press, std::bind(&APlayer::UpMove, this, std::placeholders::_1));
}

void APlayer::LeftMove(float _DeltaTime)
{
	AddActorLocation(FVector2D::LEFT * _DeltaTime * Speed);
}

void APlayer::RightMove(float _DeltaTime)
{
	AddActorLocation(FVector2D::RIGHT * _DeltaTime * Speed);
}

void APlayer::UpMove(float _DeltaTime)
{
	AddActorLocation(FVector2D::UP *_DeltaTime * Speed);
}

void APlayer::DownMove(float _DeltaTime)
{
	AddActorLocation(FVector2D::DOWN * _DeltaTime * Speed);
}

void APlayer::Tick(float _DeltaTime)
{
	//if (3.0f < UEngineInput::GetInst().GetPressTime(VK_LBUTTON))
	if (UEngineInput::GetInst().IsDown(VK_LBUTTON))
	{
		ABullet* Ptr = GetWorld()->SpawnActor<ABullet>();
		Ptr->SetActorLocation(GetActorLocation());
	}
}