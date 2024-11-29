#include "PreCompile.h"
#include "StageTitle.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include <EngineCore/SpriteRenderer.h>

AStageTitle::AStageTitle()
{
	SetActorLocation(GlobalVar::WINDOW_SIZE.Half());
}

AStageTitle::~AStageTitle()
{
}

void AStageTitle::BeginPlay()
{
	Super::BeginPlay();

	int spriteIdx = SubStage - 1;
	FVector2D size{ 150, 64 };
	SRTitle = CreateDefaultSubObject<USpriteRenderer>();
	SRTitle->SetSprite(SPRITE_STAGE_TITLE, spriteIdx);
	SRTitle->SetComponentScale(size);
	SRTitle->SetOrder(ERenderOrder::UI);

	SRTitle->CreateAnimation(ANIM_NAME, SPRITE_STAGE_TITLE, spriteIdx * 3, spriteIdx * 3 + 2, .25f);
	SRTitle->ChangeAnimation(ANIM_NAME);
}

void AStageTitle::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	static float elpasedSecs = 0.f;
	elpasedSecs += _deltaTime;

	if (elpasedSecs >= SHOWING_SECS)
	{
		elpasedSecs = 0.f;
		SRTitle->SetActive(false);
		Destroy();
	}
}