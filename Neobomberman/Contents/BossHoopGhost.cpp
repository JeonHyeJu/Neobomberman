#include "PreCompile.h"
#include "BossHoopGhost.h"
#include <EngineCore/SpriteRenderer.h>

ABossHoopGhost::ABossHoopGhost()
: AMonster()
{
	SetName("BossStage1");
}

ABossHoopGhost::~ABossHoopGhost()
{

}

void ABossHoopGhost::BeginPlay()
{
	Super::BeginPlay();
}

void ABossHoopGhost::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}

void ABossHoopGhost::InitSprite()
{
	//SRBody = CreateDefaultSubObject<USpriteRenderer>();
	//SRBody->SetSprite(SPRITE_NAME);
	//SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	//SRBody->SetComponentScale(size);
	//SRBody->SetPivotType(PivotType::Bot);
	//SRBody->SetOrder(ERenderOrder::MONSTER);
}