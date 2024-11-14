#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "Balloon.h"
#include <EngineCore/SpriteRenderer.h>

ABalloon::ABalloon()
{
	Speed = 20.f;
}

ABalloon::~ABalloon()
{

}

void ABalloon::BeginPlay()
{
	AMonster::BeginPlay();
}

void ABalloon::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);
}

void ABalloon::Init()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;

	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(SPRITE_NAME);
	SpriteRenderer->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SpriteRenderer->SetComponentScale(size);
	SpriteRenderer->SetPivotType(PivotType::Bot);
	SpriteRenderer->SetOrder(ERenderOrder::MONSTER);

	// For interface.. TODO
	SpriteRenderer->CreateAnimation("Run_Up", SPRITE_NAME, 0, 8, 0.5f);
	SpriteRenderer->CreateAnimation("Run_Down", SPRITE_NAME, 0, 8, 0.5f);
	SpriteRenderer->CreateAnimation("Run_Left", SPRITE_NAME, 0, 8, 0.5f);
	SpriteRenderer->CreateAnimation("Run_Right", SPRITE_NAME, 0, 8, 0.5f);
}