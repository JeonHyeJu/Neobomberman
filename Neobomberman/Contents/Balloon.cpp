#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "Balloon.h"
#include <EngineCore/SpriteRenderer.h>

ABalloon::ABalloon()
{

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
	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(SPRITE_NAME);
	SpriteRenderer->SetComponentLocation({ 0, 0 });
	SpriteRenderer->SetComponentScale(GlobalVar::BOMBERMAN_SIZE);
	SpriteRenderer->SetPivotType(PivotType::Bot);
	SpriteRenderer->SetOrder(ERenderOrder::PLAYER);

	SpriteRenderer->CreateAnimation("Run", SPRITE_NAME, 0, 8, 0.5f);
	SpriteRenderer->ChangeAnimation("Run");
}