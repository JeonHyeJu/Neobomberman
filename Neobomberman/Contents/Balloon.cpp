#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "Balloon.h"
#include <EngineCore/SpriteRenderer.h>

ABalloon::ABalloon()
{
	Speed = 20.f;
	SetName("Balloon");
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

void ABalloon::InitSprite()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;

	SRBody = CreateDefaultSubObject<USpriteRenderer>();
	SRBody->SetSprite(SPRITE_NAME);
	SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SRBody->SetComponentScale(size);
	SRBody->SetPivotType(PivotType::Bot);
	SRBody->SetOrder(ERenderOrder::MONSTER);

	SRBody->CreateAnimation(ANIM_RUN_UP, SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation(ANIM_RUN_DOWN, SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation(ANIM_RUN_LEFT, SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation(ANIM_RUN_RIGHT, SPRITE_NAME, 0, 8, 0.5f);

	SetScore(EMonsterScore::S200);
}

void ABalloon::ChangeMoveAnim(const FVector2D& _direction)
{
	if (_direction == FVector2D::UP)
	{
		SRBody->ChangeAnimation(ANIM_RUN_UP);
	}
	else if (_direction == FVector2D::DOWN)
	{
		SRBody->ChangeAnimation(ANIM_RUN_DOWN);
	}
	else if (_direction == FVector2D::LEFT)
	{
		SRBody->ChangeAnimation(ANIM_RUN_LEFT);
	}
	else if (_direction == FVector2D::RIGHT)
	{
		SRBody->ChangeAnimation(ANIM_RUN_RIGHT);
	}
}