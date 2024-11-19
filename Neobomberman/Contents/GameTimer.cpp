#include "PreCompile.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "GameTimer.h"
#include <EngineCore/SpriteRenderer.h>

bool AGameTimer::IsStop = true;
int AGameTimer::Seconds = 0;
const int AGameTimer::START_SECONDS = 60 + 59;

AGameTimer::AGameTimer()
{
	FVector2D size = { GlobalVar::WINDOW_SIZE.X, 44.f };
	SRBar = CreateDefaultSubObject<USpriteRenderer>();
	SRBar->SetSprite(SCORE_BAR_ADV_SPRITE_PATH);
	SRBar->SetComponentLocation({ size.hX(), 0.f });
	SRBar->SetComponentScale(size);
	SRBar->SetPivotType(PivotType::Top);
	SRBar->SetOrder(ERenderOrder::UI);

	FVector2D textSize = { 158, 14 };
	SRPushStart = CreateDefaultSubObject<USpriteRenderer>();
	SRPushStart->SetSprite(SCORE_PUSH_START_PATH);
	SRPushStart->SetComponentLocation({ 384 + textSize.hX(), 16.f });
	SRPushStart->SetComponentScale(textSize);
	SRPushStart->SetPivotType(PivotType::Top);
	SRPushStart->SetOrder(ERenderOrder::UI);

	SRPushStartBr = CreateDefaultSubObject<USpriteRenderer>();
	SRPushStartBr->SetSprite(SCORE_PUSH_START_BR_PATH);
	SRPushStartBr->SetComponentLocation({ 384 + textSize.hX(), 16.f });
	SRPushStartBr->SetComponentScale(textSize);
	SRPushStartBr->SetPivotType(PivotType::Top);
	SRPushStartBr->SetOrder(ERenderOrder::UI);
	SRPushStartBr->SetActive(false);

	SRTimerCounts.reserve(3);

	FVector2D countSize = GlobalVar::TIME_COUNT_SIZE;
	const float countMarginX = 270 + countSize.hX();
	int initIdxs[3] = { 2, 6, 10 };

	{
		USpriteRenderer* sprite = CreateDefaultSubObject<USpriteRenderer>();
		sprite->SetSprite(SCORE_TIME_COUNT, 0);
		sprite->SetComponentLocation(FVector2D{ countMarginX, 14.f } + FVector2D{ 15, 0 });
		sprite->SetComponentScale(countSize);
		sprite->SetPivotType(PivotType::Top);
		sprite->SetOrder(ERenderOrder::UI);
	}

	for (size_t i = 0; i < SRTimerCounts.capacity(); ++i)
	{
		FVector2D loc = { countMarginX, 14.f };		// First location
		if (i == 1)
		{
			loc = SRTimerCounts[i - 1]->GetComponentLocation() + FVector2D{ 30, 0 };
		}
		else if (i == 2)
		{
			loc = SRTimerCounts[i - 1]->GetComponentLocation() + FVector2D{ 16, 0 };
		}

		USpriteRenderer* sprite = CreateDefaultSubObject<USpriteRenderer>();
		sprite->SetSprite(SCORE_TIME_COUNT, initIdxs[i]);
		sprite->SetComponentLocation(loc);
		sprite->SetComponentScale(countSize);
		sprite->SetPivotType(PivotType::Top);
		sprite->SetOrder(ERenderOrder::UI);

		SRTimerCounts.push_back(sprite);
	}

	ResetTimer();
}

AGameTimer::~AGameTimer()
{
}

void AGameTimer::BeginPlay()
{
	Super::BeginPlay();

	ResetTimer();
}

void AGameTimer::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	static float elapsedTime = 0.f;
	static bool isBright = true;

	if (IsStop)
	{
		elapsedTime = 0.f;
		return;
	}

	elapsedTime += _deltaTime;
	if (elapsedTime > 1.2f)
	{
		elapsedTime = 0.f;

		Countdown();

		if (isBright)
		{
			SRPushStart->SetActive(false);
			SRPushStartBr->SetActive(true);
		}
		else
		{
			SRPushStart->SetActive(true);
			SRPushStartBr->SetActive(false);
		}

		isBright = !isBright;
	}
}

void AGameTimer::Countdown()
{
	if (Seconds < 0)
	{
		// TODO: Game over
		return;
	}

	int numer = Seconds / 60;
	int denom = Seconds % 60;
	int base10 = denom / 10;
	int base1 = denom % 10;

	SRTimerCounts[0]->SetSprite(SCORE_TIME_COUNT, numer+1);
	SRTimerCounts[1]->SetSprite(SCORE_TIME_COUNT, base10+1);
	SRTimerCounts[2]->SetSprite(SCORE_TIME_COUNT, base1+1);

	Seconds--;
}
