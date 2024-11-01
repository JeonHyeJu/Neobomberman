#include "PreCompile.h"
#include "Bomb.h"
#include "Timer.h"

#ifdef _DEBUG
#include <iostream>
#endif

#include "GlobalVar.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineBase/EngineDebug.h>

Bomb::Bomb()
{
	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(RESOURCE_PLAINBOMB_PATH);
	SpriteRenderer->SetComponentLocation({ 0, 16 });
	SpriteRenderer->SetComponentScale({ 16, 16 });

	SpriteRenderer->CreateAnimation("BombPlain_Run", RESOURCE_PLAINBOMB_PATH, 0, 4, .3f);
	SpriteRenderer->CreateAnimation("BombRed_Run", RESOURCE_REDBOMB_PATH, 0, 4, .3f);
}

Bomb::~Bomb()
{
}

void Bomb::BeginPlay()
{
	Super::BeginPlay();

	if (BombType == EBombType::PLAIN)
	{
		SpriteRenderer->ChangeAnimation("BombPlain_Run");
	}
	else if (BombType == EBombType::RED)
	{
		SpriteRenderer->ChangeAnimation("BombRed_Run");
	}
}

void Bomb::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	AccumulatedSeconds += _DeltaTime;
	if (AccumulatedSeconds >= 3.f)
	{
		Explode();
		AccumulatedSeconds = 0.f;	// temp
	}
}

void Bomb::Explode()
{
	UEngineDebug::OutPutString("BOOM!!!!!!!!!!!!!!!!!!!!!!!!!!");
}