#include "PreCompile.h"
#include "ContentsEnum.h"
#include "HoopGhost.h"
#include <EngineCore/SpriteRenderer.h>

AHoopGhost::AHoopGhost()
: AMonster()
{
	SetName("HoopGhost");
}

AHoopGhost::~AHoopGhost()
{

}

void AHoopGhost::BeginPlay()
{
	AMonster::BeginPlay();
}

void AHoopGhost::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);
}

void AHoopGhost::InitSprite()
{
	FVector2D size{ 256, 256 };
	SRBody = CreateDefaultSubObject<USpriteRenderer>();
	SRBody->SetSprite(SPRITE_NAME);
	SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SRBody->SetComponentScale(size);
	SRBody->SetPivotType(PivotType::Bot);
	SRBody->SetOrder(ERenderOrder::MONSTER);

	const float animSpeed = .25;
	SRBody->CreateAnimation(ANIM_START_HOOP, SPRITE_NAME, 0, 4, animSpeed);
	SRBody->CreateAnimation(ANIM_RUN_HOOP, SPRITE_NAME, 5, 11, animSpeed);
	SRBody->CreateAnimation(ANIM_RUN_HOOP, SPRITE_NAME, 11, 19, animSpeed);

	SetScore(EMonsterScore::S800);
}

void AHoopGhost::ChangeMoveAnim(const FVector2D& _direction)
{
	SRBody->ChangeAnimation(ANIM_RUN_HOOP);
}