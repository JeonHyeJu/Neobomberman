#include "PreCompile.h"
#include "Title.h"
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineDebug.h>

ATitle::ATitle()
{
	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite("TempTitle.png");
	SpriteRenderer->SetComponentLocation({ 151, 112 });
	SpriteRenderer->SetComponentScale({ 302, 224 });
}

ATitle::~ATitle()
{
}

void ATitle::BeginPlay()
{
	UEngineDebug::OutPutString("Temp!!!!!!!!!!!!!!!!!!!!!!!");
}

void ATitle::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (true == UEngineInput::GetInst().IsDown('R'))
	{
		UEngineAPICore::GetCore()->OpenLevel("Play");
	}
}