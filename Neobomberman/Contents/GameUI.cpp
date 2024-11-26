#include "PreCompile.h"
#include "GlobalVar.h"
#include "Player.h"
#include "ContentsEnum.h"
#include "GameData.h"
#include "GameUI.h"
#include "UtilFn.h"
#include "HurryUp.h"
#include <EngineCore/SpriteRenderer.h>
#include <EnginePlatform/EngineInput.h>
#include <EngineBase/EngineString.h>

bool AGameUI::IsStop = true;
int AGameUI::Seconds = 0;
//const int AGameUI::START_SECONDS = 5;
const int AGameUI::START_SECONDS = 60 + 59;

/* AGameUI::StatusTopBarUI */
void AGameUI::StatusTopBarUI::_SetVisibility(bool _isShow)
{
	IsShow = _isShow;

	if (SRBar) SRBar->SetActive(_isShow);
	if (SRPushStart) SRPushStart->SetActive(_isShow);
	if (SRPushStartBr) SRPushStartBr->SetActive(_isShow);
	if (SRPlayerLife) SRPlayerLife->SetActive(_isShow);
	if (SRTimerColumn) SRTimerColumn->SetActive(_isShow);
	for (int i = 0; i < 8; ++i)	// 8 temp
	{
		if (SRPlayerScore[i]) SRPlayerScore[i]->SetActive(_isShow);
	}
	for (int i = 0; i < 3; ++i)	// 3 temp
	{
		if (SRTimerCounts[i]) SRTimerCounts[i]->SetActive(_isShow);
	}
}

/* AGameUI */
AGameUI::AGameUI()
: AActor()
{
	InitLowerUI();
	InitUpperUI();
}

AGameUI::~AGameUI()
{
}

void AGameUI::BeginPlay()
{
	Super::BeginPlay();
}

void AGameUI::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	CheckInsertingCoin();
	UpdateCoinUI();
	UpdatePlayer1LifeUI();
	UpdateScoreUI();

	if (IsStop) return;

	CountDown(_deltaTime);
}

void AGameUI::UpdatePushStartBlink()
{
	if (StatusTopBar.IsHidden()) return;

	if (IsBrightPushStart)
	{
		StatusTopBar.SRPushStart->SetActive(false);
		StatusTopBar.SRPushStartBr->SetActive(true);
	}
	else
	{
		StatusTopBar.SRPushStart->SetActive(true);
		StatusTopBar.SRPushStartBr->SetActive(false);
	}
	IsBrightPushStart = !IsBrightPushStart;
}

void AGameUI::CountDown(float _deltaTime)
{
	ElapsedSecs += _deltaTime;
	if (ElapsedSecs >= 1.2f)
	{
		ElapsedSecs = 0.f;
		UpdatePushStartBlink();

		if (!IsTimeOver())
		{
			if (IsHalfTime())
			{
				AHurryUp* hurryUp = GetWorld()->SpawnActor<AHurryUp>();
			}

			UpdateTimerUI(Seconds--);
		}
	}
}

void AGameUI::LevelChangeStart()
{
	std::string curName = GetWorld()->GetName();

	if (curName == UEngineString::ToUpper("Title"))		// Temp
	{
		StatusTopBar.Hide();
	}
	else
	{
		StatusTopBar.Show();
	}

	ResetTimer();
	IsBrightPushStart = false;
}

void AGameUI::InitLowerUI()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	const float margin = 4.f;
	{
		FVector2D size = { 112, 16 };
		SRLevel_4 = CreateDefaultSubObject<USpriteRenderer>();
		SRLevel_4->SetSprite("Level-4_112x16.png");
		SRLevel_4->SetComponentScale(size);
		SRLevel_4->SetComponentLocation({ winSize.hX() + margin * 2.f, winSize.Y - size.hY() });
		SRLevel_4->SetOrder(ERenderOrder::UI_OVER_FADE);
	}

	{
		FVector2D size = { 112, 16 };
		SRCredits = CreateDefaultSubObject<USpriteRenderer>();
		SRCredits->SetSprite("Credits_112x16.png");
		SRCredits->SetComponentScale(size);
		SRCredits->SetComponentLocation({ 432 + size.hX(), winSize.Y - size.hY() });
		SRCredits->SetOrder(ERenderOrder::UI_OVER_FADE);
	}

	{
		for (int i = 0; i < 2; ++i)
		{
			FVector2D size = { 24, 24 };
			SRCreditCount[i] = CreateDefaultSubObject<USpriteRenderer>();
			SRCreditCount[i]->SetSprite(SPRITE_TIME_COUNT, 0);
			SRCreditCount[i]->SetComponentScale(size);
			SRCreditCount[i]->SetComponentLocation({ 555 + size.hX() + (i * size.X - i * margin * 1.5f), winSize.Y - size.hY() });
			SRCreditCount[i]->SetOrder(ERenderOrder::UI_OVER_FADE);
		}
	}
}

void AGameUI::InitUpperUI()
{
	FVector2D size = { GlobalVar::WINDOW_SIZE.X, 44.f };
	StatusTopBar.SRBar = CreateDefaultSubObject<USpriteRenderer>();
	StatusTopBar.SRBar->SetSprite(StatusTopBar.SCORE_BAR_ADV_SPRITE_PATH);
	StatusTopBar.SRBar->SetComponentLocation({ size.hX(), 0.f });
	StatusTopBar.SRBar->SetComponentScale(size);
	StatusTopBar.SRBar->SetPivotType(PivotType::Top);
	StatusTopBar.SRBar->SetOrder(ERenderOrder::UI);

	FVector2D textSize = { 158, 14 };
	StatusTopBar.SRPushStart = CreateDefaultSubObject<USpriteRenderer>();
	StatusTopBar.SRPushStart->SetSprite(StatusTopBar.SCORE_PUSH_START_PATH);
	StatusTopBar.SRPushStart->SetComponentLocation({ 384 + textSize.hX(), 16.f });
	StatusTopBar.SRPushStart->SetComponentScale(textSize);
	StatusTopBar.SRPushStart->SetPivotType(PivotType::Top);
	StatusTopBar.SRPushStart->SetOrder(ERenderOrder::UI);

	StatusTopBar.SRPushStartBr = CreateDefaultSubObject<USpriteRenderer>();
	StatusTopBar.SRPushStartBr->SetSprite(StatusTopBar.SCORE_PUSH_START_BR_PATH);
	StatusTopBar.SRPushStartBr->SetComponentLocation({ 384 + textSize.hX(), 16.f });
	StatusTopBar.SRPushStartBr->SetComponentScale(textSize);
	StatusTopBar.SRPushStartBr->SetPivotType(PivotType::Top);
	StatusTopBar.SRPushStartBr->SetOrder(ERenderOrder::UI);
	StatusTopBar.SRPushStartBr->SetActive(false);

	FVector2D countSize = GlobalVar::TIME_COUNT_SIZE;
	const float countMarginX = 270 + countSize.hX();
	int initIdxs[3] = { 2, 6, 10 };

	{
		StatusTopBar.SRTimerColumn = CreateDefaultSubObject<USpriteRenderer>();
		StatusTopBar.SRTimerColumn->SetSprite(StatusTopBar.SCORE_TIME_COUNT, 0);
		StatusTopBar.SRTimerColumn->SetComponentLocation(FVector2D{ countMarginX, 14.f } + FVector2D{ 15, 0 });
		StatusTopBar.SRTimerColumn->SetComponentScale(countSize);
		StatusTopBar.SRTimerColumn->SetPivotType(PivotType::Top);
		StatusTopBar.SRTimerColumn->SetOrder(ERenderOrder::UI);
	}

	int numCnt = sizeof(StatusTopBar.SRTimerCounts) / sizeof(USpriteRenderer*);
	for (int i = 0; i < numCnt; ++i)
	{
		FVector2D loc = { countMarginX, 14.f };		// First location
		if (i == 1)
		{
			loc = StatusTopBar.SRTimerCounts[i - 1]->GetComponentLocation() + FVector2D{ 30, 0 };
		}
		else if (i == 2)
		{
			loc = StatusTopBar.SRTimerCounts[i - 1]->GetComponentLocation() + FVector2D{ 16, 0 };
		}

		StatusTopBar.SRTimerCounts[i] = CreateDefaultSubObject<USpriteRenderer>();
		StatusTopBar.SRTimerCounts[i]->SetSprite(StatusTopBar.SCORE_TIME_COUNT, initIdxs[i]);
		StatusTopBar.SRTimerCounts[i]->SetComponentLocation(loc);
		StatusTopBar.SRTimerCounts[i]->SetComponentScale(countSize);
		StatusTopBar.SRTimerCounts[i]->SetPivotType(PivotType::Top);
		StatusTopBar.SRTimerCounts[i]->SetOrder(ERenderOrder::UI);
	}

	numCnt = sizeof(StatusTopBar.SRPlayerScore) / sizeof(USpriteRenderer*);
	for (int i = 0; i < numCnt; ++i)
	{
		FVector2D loc{ 134.f + i * (countSize.X * .8f), 14.f };		// First location

		StatusTopBar.SRPlayerScore[i] = CreateDefaultSubObject<USpriteRenderer>();
		StatusTopBar.SRPlayerScore[i]->SetSprite(StatusTopBar.BAR_SCORE_NUMBER_PATH);
		StatusTopBar.SRPlayerScore[i]->SetComponentLocation(loc);
		StatusTopBar.SRPlayerScore[i]->SetComponentScale(GlobalVar::BAR_SCORE_NUMBER);
		StatusTopBar.SRPlayerScore[i]->SetPivotType(PivotType::Top);
		StatusTopBar.SRPlayerScore[i]->SetOrder(ERenderOrder::UI_PLUS);
	}

	{
		FVector2D loc{ 96.f, 14.f };		// First location

		StatusTopBar.SRPlayerLife = CreateDefaultSubObject<USpriteRenderer>();
		StatusTopBar.SRPlayerLife->SetSprite(StatusTopBar.BAR_SCORE_NUMBER_PATH, 2);
		StatusTopBar.SRPlayerLife->SetComponentLocation(loc);
		StatusTopBar.SRPlayerLife->SetComponentScale(GlobalVar::BAR_SCORE_NUMBER);
		StatusTopBar.SRPlayerLife->SetPivotType(PivotType::Top);
		StatusTopBar.SRPlayerLife->SetOrder(ERenderOrder::UI);
	}
}

unsigned __int8 AGameUI::GetCoin() const
{
	return GameData::GetInstance().GetCoin();
}

void AGameUI::AddCoin(unsigned __int8 __addVal)
{
	unsigned __int8 coin = GameData::GetInstance().AddCoin(__addVal);
}

void AGameUI::CheckInsertingCoin()
{
	UEngineInput& input = UEngineInput::GetInst();

	bool isDownF3 = input.IsDown(VK_F3);
	bool isDownF4 = input.IsDown(VK_F4);

	if (isDownF3 || isDownF4)
	{
		unsigned __int8 coin = (isDownF3 ? 1 : 2);
		AddCoin(coin);
	}
}

void AGameUI::UpdateCoinUI()
{
	unsigned __int8 coin = GameData::GetInstance().GetCoin();

	if (PrevCoin != coin)
	{
		PrevCoin = coin;

		int base10 = coin / 10;
		int base1 = coin % 10;

		SRCreditCount[0]->SetSprite(SPRITE_TIME_COUNT, base10);
		SRCreditCount[1]->SetSprite(SPRITE_TIME_COUNT, base1);
	}
}

void AGameUI::UpdateScoreUI()
{
	if (StatusTopBar.IsHidden()) return;

	int p1Score = GameData::GetInstance().GetPlayer1Score();

	if (PrevScore != p1Score)
	{
		PrevScore = p1Score;

		int size = sizeof(StatusTopBar.SRPlayerScore) / sizeof(USpriteRenderer*);
		std::vector<int>&& scoreVec = UtilFn::IntToVector(p1Score, size);

		for (int i = size - 1; i >= 0; --i)
		{
			int val = scoreVec[i];
			if (val != -1)
			{
				StatusTopBar.SRPlayerScore[i]->SetSprite(StatusTopBar.BAR_SCORE_NUMBER_PATH, val);
				StatusTopBar.SRPlayerScore[i]->SetActive(true);
			}
			else
			{
				StatusTopBar.SRPlayerScore[i]->SetActive(false);
			}
		}
	}
}

void AGameUI::UpdatePlayer1LifeUI()
{
	if (StatusTopBar.IsHidden()) return;

	int p1Life = GameData::GetInstance().GetPlayer1Life();
	if (PrevP1Life != p1Life)
	{
		PrevP1Life = p1Life;

		if (p1Life >= 0)
		{
			StatusTopBar.SRPlayerLife->SetSprite(StatusTopBar.BAR_SCORE_NUMBER_PATH, p1Life);
		}
	}
}

void AGameUI::UpdateTimerUI(int _seconds)
{
	if (StatusTopBar.IsHidden()) return;

	int numer = _seconds / 60;
	int denom = _seconds % 60;
	int base10 = denom / 10;
	int base1 = denom % 10;

	StatusTopBar.SRTimerCounts[0]->SetSprite(StatusTopBar.SCORE_TIME_COUNT, numer + 1);
	StatusTopBar.SRTimerCounts[1]->SetSprite(StatusTopBar.SCORE_TIME_COUNT, base10 + 1);
	StatusTopBar.SRTimerCounts[2]->SetSprite(StatusTopBar.SCORE_TIME_COUNT, base1 + 1);
}