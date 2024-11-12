#include "PreCompile.h"
#include "Mushroom.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include <EngineCore/SpriteRenderer.h>

AMushroom::AMushroom()
{
}

AMushroom::~AMushroom()
{
}

void AMushroom::BeginPlay()
{
	AMonster::BeginPlay();
}

void AMushroom::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);

	// TODO: change to FSM
	if (State == EMushroomState::INIT_BLINK)
	{
		AccumulatedSecs += _deltaTime;
		if (AccumulatedSecs >= BLINK_SECONDS)
		{
			SpriteRenderer->SetActive(true);
			State = EMushroomState::WALKING;
			return;
		}

		SpriteRenderer->SetActiveSwitch();
	}
	else if (State == EMushroomState::WALKING)
	{
		// Nothing..
	}
	else if (State == EMushroomState::JUMPING)
	{

	}
}

void AMushroom::Init()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;

	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(SPRITE_NAME);
	SpriteRenderer->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SpriteRenderer->SetComponentScale(size);
	SpriteRenderer->SetPivotType(PivotType::Bot);
	SpriteRenderer->SetOrder(ERenderOrder::PLAYER);

	SpriteRenderer->CreateAnimation("Idle_Up", SPRITE_NAME, 0, 0, 0.1f);
	SpriteRenderer->CreateAnimation("Idle_Down", SPRITE_NAME, 22, 22, 0.1f);
	SpriteRenderer->CreateAnimation("Idle_Left", SPRITE_NAME, 33, 33, 0.1f);
	SpriteRenderer->CreateAnimation("Idle_Right", SPRITE_NAME, 11, 11, 0.1f);

	SpriteRenderer->CreateAnimation("Run_Up", SPRITE_NAME, 1, 6, 0.1f);
	SpriteRenderer->CreateAnimation("Run_Down", SPRITE_NAME, 23, 28, 0.1f);
	SpriteRenderer->CreateAnimation("Run_Left", SPRITE_NAME, 34, 39, 0.1f);
	SpriteRenderer->CreateAnimation("Run_Right", SPRITE_NAME, 12, 17, 0.1f);

	std::vector<int> indexes;
	std::vector<float> times;

	// temp. TODO
	int startIdx = 44;
	int endIdx = 54;
	int subIdx = endIdx - startIdx;
	indexes.reserve(subIdx);
	times.reserve(subIdx);
	for (int i = startIdx; i < endIdx; i++)
	{
		indexes.push_back(i);
		if (i >= startIdx+3 && i < endIdx-3)
		{
			times.push_back(1.f);
		}
		else
		{
			times.push_back(0.3f);
		}
		
	}
	SpriteRenderer->CreateAnimation("Jump", SPRITE_NAME, indexes, times);

	//SpriteRenderer->ChangeAnimation("Idle_Down");
	SpriteRenderer->ChangeAnimation("Jump");

	State = EMushroomState::INIT_BLINK;
}