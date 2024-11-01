#include "PreCompile.h"
#include "BombPlain.h"
#include "GlobalVar.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineBase/EngineDebug.h>

BombPlain::BombPlain()
: Bomb()
{
	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(RESOURCE_PATH);
	SpriteRenderer->SetComponentLocation({ 0, 16 });
	SpriteRenderer->SetComponentScale({ 16, 16 });

	SpriteRenderer->CreateAnimation("BombPlain_Run", RESOURCE_PATH, 0, 2, .3f);
}

BombPlain::~BombPlain()
{
}

void BombPlain::BeginPlay()
{
	Super::BeginPlay();

	SpriteRenderer->ChangeAnimation("BombPlain_Run");
}

void BombPlain::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	AccumulatedSeconds += _DeltaTime;
	if (AccumulatedSeconds >= 3.f)
	{
		UEngineDebug::OutPutString("BOOM!!!!!!!!!!!!!!!!!!!!!!!!!!");
		AccumulatedSeconds = 0.f;	// temp
	}
}