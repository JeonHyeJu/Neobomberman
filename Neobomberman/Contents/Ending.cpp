#include "PreCompile.h"
#include "Ending.h"
#include "GlobalVar.h"
#include "Fade.h"
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineAPICore.h>

AEnding::AEnding()
{
	FVector2D size{ 511, 256 };
	const int ALL_FRAME_CNT = 156;
	const float SHORT_ANIM_SEC = .125f;
	const float LONG_ANIM_SEC = 2.f;

	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(ENDING_SPRITE_PATH);
	SpriteRenderer->SetComponentLocation(GlobalVar::WINDOW_SIZE.Half());
	SpriteRenderer->SetComponentScale(size);
}

AEnding::~AEnding()
{
}

void AEnding::BeginPlay()
{
}

void AEnding::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	if (UEngineInput::GetInst().IsDown(VK_SPACE) || UEngineInput::GetInst().IsDown(VK_RETURN))
	{
		AFade::MainFade->BindEndEvent(std::bind(&AEnding::OnEndFadeOut, this));
		AFade::MainFade->FadeOut();
	}
}

void AEnding::OnEndFadeOut()
{
	UEngineAPICore::GetCore()->OpenLevel("Title");
}