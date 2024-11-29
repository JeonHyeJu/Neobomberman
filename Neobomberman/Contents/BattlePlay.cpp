#include "PreCompile.h"
#include "BattlePlay.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include <EngineCore/SpriteRenderer.h>

ABattlePlay::ABattlePlay()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	SRBg = CreateDefaultSubObject<USpriteRenderer>();
	SRBg->SetSprite(SPRITE_BATTLE_BG);
	SRBg->SetComponentLocation(winSize.Half());
	SRBg->SetComponentScale(winSize);
	//SRBg->CreateAnimation(ANIM_NAME, SPRITE_PATH, 0, 4, .1f);
	SRBg->SetOrder(ERenderOrder::BACKGROUND);
}

ABattlePlay::~ABattlePlay()
{

}

void ABattlePlay::BeginPlay()
{
	Super::BeginPlay();
}

void ABattlePlay::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}
