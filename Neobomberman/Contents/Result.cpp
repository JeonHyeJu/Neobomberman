#include "PreCompile.h"
#include "Result.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "UtilFn.h"
#include <EngineCore/Actor.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineAPICore.h>

AResult::AResult()
{
	{
		FVector2D size{ 256, 128 };
		SRStageClear = CreateDefaultSubObject<USpriteRenderer>();
		SRStageClear->SetSprite("StageClear");
		SRStageClear->SetComponentLocation(FVector2D{ 184, 92 } + size.Half());
		SRStageClear->SetComponentScale(size);
		SRStageClear->SetOrder(ERenderOrder::RESULT);
		SRStageClear->CreateAnimation("Blinking", "StageClear", 0, 11, .75f);
	}

	{
		FVector2D size = GlobalVar::WINDOW_SIZE;
		SRResultFrame = CreateDefaultSubObject<USpriteRenderer>();
		SRResultFrame->SetSprite("ResultFrame.png");
		SRResultFrame->SetComponentLocation(size.Half());
		SRResultFrame->SetComponentScale(size);
		SRResultFrame->SetOrder(ERenderOrder::RESULT);
	}

	{
		FVector2D size{ 256, 256 };
		SRResultRImage = CreateDefaultSubObject<USpriteRenderer>();
		SRResultRImage->SetSprite("ResultImage1.png");
		SRResultRImage->SetComponentLocation(LastLocRImage + size.Half() + FVector2D{ GlobalVar::WINDOW_SIZE.X, 0.f });
		SRResultRImage->SetComponentScale(size);
		SRResultRImage->SetOrder(ERenderOrder::RESULT);
	}

	{
		FVector2D size{ 256, 256 };
		SRStamp = CreateDefaultSubObject<USpriteRenderer>();
		SRStamp->SetSprite("GoodStamp.png");
		SRStamp->SetComponentLocation(FVector2D{ 82, 190 } + size.Half());
		SRStamp->SetComponentScale(size);
		SRStamp->SetOrder(ERenderOrder::RESULT_STAMP);
	}

	{
		FVector2D size{ 32, 32 };

		for (int i = 0; i < 3; ++i)
		{
			int addVal = (i > 0 ? static_cast<int>(size.X) : 0);
			SRTime[i] = CreateDefaultSubObject<USpriteRenderer>();
			SRTime[i]->SetSprite(RESULT_COUNT_PATH);
			SRTime[i]->SetComponentLocation(FVector2D{ 158 + addVal, 240 } + size.Half() + FVector2D{ i * size.X, 0.f });
			SRTime[i]->SetComponentScale(size);
			SRTime[i]->SetOrder(ERenderOrder::RESULT);
		}

		for (int i = 0; i < SHOW_NUMBER_CNT; ++i)
		{
			SRBonus[i] = CreateDefaultSubObject<USpriteRenderer>();
			SRBonus[i]->SetSprite(RESULT_COUNT_PATH);
			SRBonus[i]->SetComponentLocation(FVector2D{ 30, 306 } + size.Half() + FVector2D{ i * size.X, 0.f });
			SRBonus[i]->SetComponentScale(size);
			SRBonus[i]->SetOrder(ERenderOrder::RESULT);
		}

		for (int i = 0; i < SHOW_NUMBER_CNT; ++i)
		{
			SRTotal[i] = CreateDefaultSubObject<USpriteRenderer>();
			SRTotal[i]->SetSprite(RESULT_COUNT_PATH);
			SRTotal[i]->SetComponentLocation(FVector2D{ 30, 368 } + size.Half() + FVector2D{ i * size.X, 0.f });
			SRTotal[i]->SetComponentScale(size);
			SRTotal[i]->SetOrder(ERenderOrder::RESULT);
		}
	}

	SRStamp->SetActive(false);
	SRResultRImage->SetActive(false);

	FSM.CreateState(EResultState::INIT, nullptr);
	FSM.CreateState(EResultState::MOVE, std::bind(&AResult::Moving, this, std::placeholders::_1));
	FSM.CreateState(EResultState::CALCULATE_BONUS, std::bind(&AResult::CalculatingBonus, this, std::placeholders::_1));
	FSM.CreateState(EResultState::CALCULATE_TOTAL, std::bind(&AResult::CalculatingTotal, this, std::placeholders::_1));
	FSM.CreateState(EResultState::STAMP, std::bind(&AResult::Stamping, this, std::placeholders::_1));
	FSM.CreateState(EResultState::SHOW_RIMAGE, std::bind(&AResult::ShowingRImage, this, std::placeholders::_1));
	FSM.CreateState(EResultState::SHAKE_RIMAGE, std::bind(&AResult::ShakingRImage, this, std::placeholders::_1));

	FSM.ChangeState(EResultState::INIT);
}

AResult::~AResult()
{
}

void AResult::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation({ -GlobalVar::WINDOW_SIZE.X, 0.f });
	SetLastTimeUI(LastSecs);

	HideAllBonusUI();
	HideAllTotalUI();

	SetBonusUI(Bonus);
	SetTotalUI(Total);

	SRStageClear->ChangeAnimation("Blinking");

	FSM.ChangeState(EResultState::MOVE);
}

void AResult::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	FSM.Update(_deltaTime);
}

void AResult::SetLastTimeUI(int _lastSecs)
{
	LastSecs = _lastSecs;

	int minutes = _lastSecs / 60;
	int seconds = _lastSecs % 60;
	int base10 = seconds / 10;
	int base1 = seconds % 10;

	SRTime[0]->SetSprite(RESULT_COUNT_PATH, minutes);
	SRTime[1]->SetSprite(RESULT_COUNT_PATH, base10);
	SRTime[2]->SetSprite(RESULT_COUNT_PATH, base1);
}

void AResult::SetBonusUI(int _bonus)
{
	static int PrevBonus = -1;

	if (_bonus != PrevBonus)
	{
		PrevBonus = _bonus;
		Bonus = _bonus;

		int size = sizeof(SRBonus) / sizeof(USpriteRenderer*);
		std::vector<int>&& bonusVec = UtilFn::IntToVector(_bonus, size);

		for (int i = size - 1; i >= 0; --i)
		{
			int val = bonusVec[i];
			if (val != -1)
			{
				SRBonus[i]->SetSprite(RESULT_COUNT_PATH, val);
				SRBonus[i]->SetActive(true);
			}
			else
			{
				SRBonus[i]->SetActive(false);
			}
		}
	}
}

void AResult::SetTotalUI(int _total)
{
	static int PrevTotal = -1;

	if (_total != PrevTotal)
	{
		PrevTotal = _total;
		Total = _total;

		int size = sizeof(SRTotal) / sizeof(USpriteRenderer*);
		std::vector<int>&& bonusVec = UtilFn::IntToVector(_total, size);

		for (int i = size - 1; i >= 0; --i)
		{
			int val = bonusVec[i];
			if (val != -1)
			{
				SRTotal[i]->SetSprite(RESULT_COUNT_PATH, val);
				SRTotal[i]->SetActive(true);
			}
			else
			{
				SRTotal[i]->SetActive(false);
			}
		}
	}
}

void AResult::HideAllBonusUI()
{
	for (int i = 0; i < SHOW_NUMBER_CNT; ++i)
	{
		SRBonus[i]->SetActive(false);
	}
}

void AResult::HideAllTotalUI()
{
	for (int i = 0; i < SHOW_NUMBER_CNT; ++i)
	{
		SRTotal[i]->SetActive(false);
	}
}

void AResult::Moving(float _deltaTime)
{
	if (GetActorLocation().X < 0.f)
	{
		AddActorLocation({ Speed * _deltaTime, 0.f });
	}
	else
	{
		FSM.ChangeState(EResultState::CALCULATE_BONUS);
	}
}

void AResult::CalculatingBonus(float _deltaTime)
{
	ElapsedSecs += _deltaTime;

	if (ElapsedSecs > .01f)
	{
		ElapsedSecs = 0.f;

		if (LastSecs > 0)
		{
			SetLastTimeUI(LastSecs - 1);
			SetBonusUI(Bonus + 50);
		}
		else
		{
			FSM.ChangeState(EResultState::CALCULATE_TOTAL);
		}
	}
}

void AResult::CalculatingTotal(float _deltaTime)
{
	ElapsedSecs += _deltaTime;

	if (ElapsedSecs > 1.f)
	{
		ElapsedSecs = 0.f;

		if (Bonus > 0)
		{
			SetTotalUI(Total + Bonus);
			SetBonusUI(0);
		}
		else
		{
			FSM.ChangeState(EResultState::STAMP);
		}
	}
}

void AResult::Stamping(float _deltaTime)
{
	ElapsedSecs += _deltaTime;

	if (ElapsedSecs > .75f)
	{
		ElapsedSecs = 0.f;

		if (!SRStamp->IsActive())
		{
			SRStamp->SetActive(true);
		}
		else
		{
			SRResultRImage->SetActive(true);
			FSM.ChangeState(EResultState::SHOW_RIMAGE);
		}
	}
}

void AResult::ShowingRImage(float _deltaTime)
{
	FVector2D loc = SRResultRImage->GetComponentLocation();

	if (loc.X > LastLocRImage.X)
	{
		SRResultRImage->SetComponentLocation(loc - FVector2D{ SpeedRImage* _deltaTime, 0.f });
	}
	else
	{
		FSM.ChangeState(EResultState::SHAKE_RIMAGE);
	}
}

void AResult::ShakingRImage(float _deltaTime)
{
	static float allEpalsedTime = 0.f;
	static float addVals[6] = { -1.f, -1.f, -1.f, 1.f, 1.f, 1.f };
	static int addIdx = -1;

	ElapsedSecs += _deltaTime;
	allEpalsedTime += _deltaTime;

	if (ElapsedSecs > .5f)
	{
		ElapsedSecs = 0.f;

		FVector2D loc = SRResultRImage->GetComponentLocation();

		// Temp
		addIdx = (addIdx + 1) % 6;
		SRResultRImage->SetComponentLocation(loc + FVector2D{ 0.f, addVals[addIdx] });

		if (allEpalsedTime > 5.f)
		{
			// TODO: GO TO THE NEXT
			FSM.ChangeState(EResultState::INIT);

			// TODO: here?
			UEngineAPICore::GetCore()->OpenLevel("Boss_Stage1");
		}
	}
}