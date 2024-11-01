#include "PreCompile.h"
#include "TitleLogo.h"
#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EnginePlatform/EngineInput.h>

TitleLogo::TitleLogo()
{
	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite("TempTitle.png");
	SpriteRenderer->SetComponentLocation({ 151, 112 });
	SpriteRenderer->SetComponentScale({ 302, 224 });
}

TitleLogo::~TitleLogo()
{
	
}

void TitleLogo::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (true == UEngineInput::GetInst().IsDown('R'))
	{
		UEngineAPICore::GetCore()->OpenLevel("Play");
	}
}