#include "PreCompile.h"
#include "Bullet.h"

ABullet::ABullet()
{
	SetActorScale({ 50, 50 });
}

ABullet::~ABullet()
{
}

void ABullet::Tick(float _DeltaTime)
{
	AddActorLocation(FVector2D::RIGHT * _DeltaTime * 300.0f);
}