#include "PreCompile.h"
#include "Title.h"
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineString.h>

ATitle::ATitle()
{
	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(RESOURCE_PATH);
	SpriteRenderer->SetComponentLocation({ 151, 112 });
	SpriteRenderer->SetComponentScale({ 302, 224 });

	const int ALL_FRAME = 156;
	const float SHORT_ANIM_SEC = .125f;
	const float LONG_ANIM_SEC = 2.f;

	std::vector<int> indexes;
	std::vector<float> frames(ALL_FRAME, SHORT_ANIM_SEC);

	indexes.reserve(ALL_FRAME);
	frames.reserve(ALL_FRAME);

	for (int i = 0; i < ALL_FRAME; ++i)
	{
		indexes.push_back(i);
	}
	frames[0] = LONG_ANIM_SEC;

	SpriteRenderer->CreateAnimation(ANIM_IDLE_NAME, RESOURCE_PATH, ALL_FRAME, ALL_FRAME, 0.1f);

	SpriteRenderer->CreateAnimation(ANIM_RUN_NAME, RESOURCE_PATH, indexes, frames, false);
	SpriteRenderer->SetAnimationEvent(ANIM_RUN_NAME, ALL_FRAME-1, std::bind(&ATitle::OnEndAnimation, this));
}

ATitle::~ATitle()
{
}

void ATitle::BeginPlay()
{
	SpriteRenderer->ChangeAnimation(ANIM_RUN_NAME);
}

void ATitle::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (true == UEngineInput::GetInst().IsDown('R'))
	{
		std::string&& nowAnimName = SpriteRenderer->GetNowAnimation();
		if (nowAnimName == UEngineString::ToUpper(ANIM_RUN_NAME))
		{
			OnEndAnimation();
		}
		else
		{
			UEngineAPICore::GetCore()->OpenLevel("Play");
		}
	}
}

void ATitle::OnEndAnimation()
{
	SpriteRenderer->ChangeAnimation(ANIM_IDLE_NAME);
}