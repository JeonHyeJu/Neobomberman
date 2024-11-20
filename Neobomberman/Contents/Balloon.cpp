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

void ABalloon::Init()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;

	SRBody = CreateDefaultSubObject<USpriteRenderer>();
	SRBody->SetSprite(SPRITE_NAME);
	SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SRBody->SetComponentScale(size);
	SRBody->SetPivotType(PivotType::Bot);
	SRBody->SetOrder(ERenderOrder::MONSTER);

	// For interface.. TODO
	SRBody->CreateAnimation("Run_Up", SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation("Run_Down", SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation("Run_Left", SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation("Run_Right", SPRITE_NAME, 0, 8, 0.5f);

	{
		FVector2D size = GlobalVar::BOMB_SIZE;

		SRScore = CreateDefaultSubObject<USpriteRenderer>();
		SRScore->SetSprite(MONSTER_SCORE_PATH);
		SRScore->SetComponentLocation(size.Half().Half());
		SRScore->SetComponentScale(size);
		SRScore->CreateAnimation("Disappear", MONSTER_SCORE_PATH, 0, 5, .25f, false);
		SRScore->SetActive(false);
	}

	Score = EMonsterScore::S200;
}

void ABalloon::ShowScore()
{
	SRScore->ChangeAnimation("Disappear");
}
