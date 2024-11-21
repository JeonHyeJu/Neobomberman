#include "PreCompile.h"
#include "BossStage1.h"
#include <EngineCore/SpriteRenderer.h>

BossStage1::BossStage1()
: AMonster()
{
	SetName("BossStage1");
}

BossStage1::~BossStage1()
{

}

void BossStage1::BeginPlay()
{
	Super::BeginPlay();
}

void BossStage1::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}

void BossStage1::Init()
{
	//SRBody = CreateDefaultSubObject<USpriteRenderer>();
	//SRBody->SetSprite(SPRITE_NAME);
	//SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	//SRBody->SetComponentScale(size);
	//SRBody->SetPivotType(PivotType::Bot);
	//SRBody->SetOrder(ERenderOrder::MONSTER);
}