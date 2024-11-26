#include "PreCompile.h"
#include "HurryUp.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include <EngineCore/SpriteRenderer.h>

AHurryUp::AHurryUp()
{
}

AHurryUp::~AHurryUp()
{
}

void AHurryUp::BeginPlay()
{
	Super::BeginPlay();

	FVector2D winSize = GlobalVar::WINDOW_SIZE;

	Size = { 510, 64 };	// Temp
	SRHurryUp = CreateDefaultSubObject<USpriteRenderer>();
	SRHurryUp->SetSprite(SPRITE_PATH);
	SRHurryUp->SetComponentLocation(winSize.Half());
	SRHurryUp->SetComponentScale(Size);
	SRHurryUp->CreateAnimation(ANIM_NAME, SPRITE_PATH, 0, 4, .1f);
	SRHurryUp->SetOrder(ERenderOrder::UI);

	SRHurryUp->ChangeAnimation(ANIM_NAME);

	SetActorLocation(FVector2D{ winSize.X, 0.f });
}

void AHurryUp::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	if (IsStop)
	{
		if (!IsDestoiable)
		{
			IsDestoiable = true;
			Destroy();
		}
		return;
	}

	static float elapsedSecs = 0.f;
	elapsedSecs += _deltaTime;
	if (elapsedSecs > .01f)
	{
		elapsedSecs = 0.f;

		AddActorLocation(FVector2D::LEFT * _deltaTime * Speed);
		FVector2D loc = GetActorLocation();
		//OutputDebugString(("loc: " + std::to_string(loc.X) + ", " + std::to_string(loc.Y) + "\n").c_str());

		if (loc.X < -(Size.X + 128.f))	// with margin
		{
			IsStop = true;
			SRHurryUp->SetActive(false);
		}
	}
}
