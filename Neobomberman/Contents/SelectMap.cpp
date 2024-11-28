#include "PreCompile.h"
#include "SelectMap.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include <EngineCore/SpriteRenderer.h>

ASelectMap::ASelectMap()
{
}

ASelectMap::~ASelectMap()
{
}

void ASelectMap::BeginPlay()
{
	Super::BeginPlay();

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	SRBackground = CreateDefaultSubObject<USpriteRenderer>();
	SRBackground->SetComponentLocation(winSize.Half());
	SRBackground->SetComponentScale(winSize);
	SRBackground->SetSprite("SelectMap.png");
	SRBackground->SetOrder(ERenderOrder::BACKGROUND);

	SRSirenL = CreateDefaultSubObject<USpriteRenderer>();
	SRSirenL->SetComponentLocation(FVector2D{ 180, 170 });
	SRSirenL->SetComponentScale(FVector2D{ 32, 32 });
	SRSirenL->SetSprite("Siren.png");
	SRSirenL->SetOrder(ERenderOrder::BACKGROUND_OVER);
	SRSirenL->CreateAnimation("Alert", "Siren.png", 0, 3, .2f);

	SRSirenR = CreateDefaultSubObject<USpriteRenderer>();
	SRSirenR->SetComponentLocation(FVector2D{ 428, 170 });
	SRSirenR->SetComponentScale(FVector2D{ 32, 32 });
	SRSirenR->SetSprite("Siren.png");
	SRSirenR->SetOrder(ERenderOrder::BACKGROUND_OVER);
	SRSirenR->CreateAnimation("Alert", "Siren.png", 0, 3, .2f);

	SRSirenL->ChangeAnimation("Alert");
	SRSirenR->ChangeAnimation("Alert");
}

void ASelectMap::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}
