#include "PreCompile.h"
#include "Title.h"
#include "GlobalVar.h"
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineString.h>

ATitle::ATitle()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	const int ALL_FRAME_CNT = 156;
	const float SHORT_ANIM_SEC = .125f;
	const float LONG_ANIM_SEC = 2.f;

	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(GlobalPath::OPENING);
	SpriteRenderer->SetComponentLocation(winSize * .5);
	SpriteRenderer->SetComponentScale(winSize);

	std::vector<int> indexes;
	std::vector<float> frames(ALL_FRAME_CNT, SHORT_ANIM_SEC);

	indexes.reserve(ALL_FRAME_CNT);
	frames.reserve(ALL_FRAME_CNT);

	for (int i = 0; i < ALL_FRAME_CNT; ++i)
	{
		indexes.push_back(i);
	}
	frames[0] = LONG_ANIM_SEC;

	SpriteRenderer->CreateAnimation(ANIM_IDLE_NAME, GlobalPath::OPENING, ALL_FRAME_CNT, ALL_FRAME_CNT, 0.1f);
	SpriteRenderer->CreateAnimation(ANIM_RUN_NAME, GlobalPath::OPENING, indexes, frames, false);

	SpriteRenderer->SetAnimationEvent(ANIM_RUN_NAME, ALL_FRAME_CNT-1, std::bind(&ATitle::OnEndAnimation, this));
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
		std::string nowAnimName = UEngineString::ToUpper(SpriteRenderer->GetCurAnimName());
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