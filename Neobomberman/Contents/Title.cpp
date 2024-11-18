#include "PreCompile.h"
#include "Title.h"
#include "GlobalVar.h"
#include "Fade.h"
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineString.h>

ATitle::ATitle()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	{
		const int ALL_FRAME_CNT = 155;
		const float SHORT_ANIM_SEC = .125f;
		const float LONG_ANIM_SEC = 2.f;

		SROpening = CreateDefaultSubObject<USpriteRenderer>();
		SROpening->SetSprite(GlobalPath::OPENING);
		SROpening->SetComponentLocation(winSize.Half());
		SROpening->SetComponentScale(winSize);

		std::vector<int> indexes;
		std::vector<float> frames(ALL_FRAME_CNT, SHORT_ANIM_SEC);

		indexes.reserve(ALL_FRAME_CNT);
		frames.reserve(ALL_FRAME_CNT);

		for (int i = 0; i < ALL_FRAME_CNT; ++i)
		{
			indexes.push_back(i);
		}
		frames[0] = LONG_ANIM_SEC;

		SROpening->CreateAnimation(ANIM_IDLE_NAME, GlobalPath::OPENING, ALL_FRAME_CNT, ALL_FRAME_CNT, 0.1f);
		SROpening->CreateAnimation(ANIM_RUN_NAME, GlobalPath::OPENING, indexes, frames, false);

		SROpening->SetAnimationEvent(ANIM_RUN_NAME, ALL_FRAME_CNT - 1, std::bind(&ATitle::OnEndAnimation, this));
	}

	const float margin = 4.f;
	// Temp location and scale below all
	{
		FVector2D size = { 304, 16 };
		SRPushP1P2 = CreateDefaultSubObject<USpriteRenderer>();
		SRPushP1P2->SetSprite("Push_p1p2_304x16.png");
		SRPushP1P2->SetComponentScale(size);
		SRPushP1P2->SetComponentLocation({ winSize.hX() + margin * 6.8f, 289.f });
	}

	{
		FVector2D size = { 64, 16 };
		SRTimeTitle = CreateDefaultSubObject<USpriteRenderer>();
		SRTimeTitle->SetSprite("Time_64x16.png");
		SRTimeTitle->SetComponentScale(size);
		SRTimeTitle->SetComponentLocation({ 288 + size.hX(), 354.f });
	}

	{
		for (int i = 0; i < 2; ++i)
		{
			FVector2D size = { 24, 24 };
			SRTimeCount[i] = CreateDefaultSubObject<USpriteRenderer>();
			if (i == 0)
			{
				SRTimeCount[i]->SetSprite(SPRITE_TIME_COUNT, 3);
			}
			else
			{
				SRTimeCount[i]->SetSprite(SPRITE_TIME_COUNT, 0);
			}
			SRTimeCount[i]->SetComponentScale(size);
			SRTimeCount[i]->SetComponentLocation({
				SRTimeTitle->GetComponentLocation().X - 6 + (i * size.X - i * margin * 1.5f),
				SRTimeTitle->GetComponentLocation().Y + SRTimeTitle->GetComponentScale().Y
			});
		}
	}

	{
		FVector2D size = { 112, 16 };
		SRLevel_4 = CreateDefaultSubObject<USpriteRenderer>();
		SRLevel_4->SetSprite("Level-4_112x16.png");
		SRLevel_4->SetComponentScale(size);
		SRLevel_4->SetComponentLocation({ winSize.hX() + margin * 2.f, winSize.Y - size.hY()});
	}

	{
		FVector2D size = { 112, 16 };
		SRCredits = CreateDefaultSubObject<USpriteRenderer>();
		SRCredits->SetSprite("Credits_112x16.png");
		SRCredits->SetComponentScale(size);
		SRCredits->SetComponentLocation({ 432 + size.hX(), winSize.Y - size.hY()});
	}

	{
		for (int i = 0; i < 2; ++i)
		{
			FVector2D size = { 24, 24 };
			SRCreditCount[i] = CreateDefaultSubObject<USpriteRenderer>();
			SRCreditCount[i]->SetSprite(SPRITE_TIME_COUNT, 0);
			SRCreditCount[i]->SetComponentScale(size);
			SRCreditCount[i]->SetComponentLocation({ 555 + size.hX() + (i * size.X - i * margin * 1.5f), winSize.Y - size.hY() });
		}
	}

	{
		SRSelectGameMode = CreateDefaultSubObject<USpriteRenderer>();
		SRSelectGameMode->SetSprite("SelectGameMode.png");
		SRSelectGameMode->SetComponentLocation(winSize.Half());
		SRSelectGameMode->SetComponentScale(winSize);
	}

	{
		for (int i = 0; i < 2; ++i)
		{
			FVector2D size = { 24, 48 };
			SRSelectTimeCount[i] = CreateDefaultSubObject<USpriteRenderer>();
			if (i == 0)
			{
				SRSelectTimeCount[i]->SetSprite(SPRITE_SELECT_TIME_COUNT_B, 3);
			}
			else
			{
				SRSelectTimeCount[i]->SetSprite(SPRITE_SELECT_TIME_COUNT_B, 0);

			}
			SRSelectTimeCount[i]->SetComponentScale(size);
			SRSelectTimeCount[i]->SetComponentLocation({ 497.f + i * (size.X + 5), 104.f });
		}
	}

	{
		const int ALL_FRAME_CNT = 7;
		const float SHORT_ANIM_SEC = .5f;
		const float LONG_ANIM_SEC = 1.5f;

		std::vector<int> indexes;
		std::vector<float> times(ALL_FRAME_CNT, SHORT_ANIM_SEC);

		indexes.reserve(ALL_FRAME_CNT);
		times.reserve(ALL_FRAME_CNT);

		for (int i = 0; i < ALL_FRAME_CNT; ++i)
		{
			indexes.push_back(i);
		}
		times[0] = LONG_ANIM_SEC * 2;
		times[times.size() - 1] = LONG_ANIM_SEC;

		FVector2D size = { 54, 96 };
		SRSelectSayHi = CreateDefaultSubObject<USpriteRenderer>();
		SRSelectSayHi->SetSprite("SayHi");
		SRSelectSayHi->SetComponentLocation({ 354.f + size.hX(), 16.f + size.hY() });
		SRSelectSayHi->SetComponentScale(size);
		SRSelectSayHi->CreateAnimation("SayHi", "SayHi", indexes, times);
	}

	{
		FVector2D size = GlobalVar::BOMBERMAN_SIZE;
		SRSelectPainter = CreateDefaultSubObject<USpriteRenderer>();
		SRSelectPainter->SetSprite("SelectPainter_idle.png");
		SRSelectPainter->SetComponentLocation(PAINTER_START_LOC);
		SRSelectPainter->SetComponentScale(size);

		SRSelectPainter->CreateAnimation("GoingUpDown", "SelectPainterUpDown.png", 0, 6, .3f, false);
		SRSelectPainter->CreateAnimation("DrawCircle", "SelectPainterCircle.png", 0, 8, .1f, false);
		SRSelectPainter->SetAnimationEvent("DrawCircle", 8, std::bind(&ATitle::OnEndPainterDraw, this));
	}

	{
		FVector2D size = { 128, 128 };
		SRSelectCircle = CreateDefaultSubObject<USpriteRenderer>();
		SRSelectCircle->SetSprite("BrushCircle.png");
		SRSelectCircle->SetComponentLocation({ 140, 224 });
		SRSelectCircle->SetComponentScale(size);

		SRSelectCircle->CreateAnimation("DrawCircle", "BrushCircle.png", 0, 6, .1f, false);
		SRSelectCircle->SetAnimationEvent("DrawCircle", 6, std::bind(&ATitle::OnEndCircleDraw, this));
		SRSelectCircle->SetActive(false);
	}

	SwitchStartUi(false);
	SwitchSelectUi(false);
}

ATitle::~ATitle()
{
}

// Temp
void ATitle::LevelChangeStart()
{
	// Temp
	AFade::MainFade->BindEndEvent(std::bind(&ATitle::OnEndFadeOut, this));
}

void ATitle::BeginPlay()
{
	Super::BeginPlay();

	SROpening->ChangeAnimation(ANIM_RUN_NAME);
	ResetSeconds();
}

void ATitle::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	// TODO: move to FSM
	static bool isWaitToStart = false;
	static bool isWaitToSelect = false;
	bool isDownF1 = UEngineInput::GetInst().IsDown(VK_F1);
	bool isDownF3 = UEngineInput::GetInst().IsDown(VK_F3);
	bool isDownF4 = UEngineInput::GetInst().IsDown(VK_F4);
	bool isDownA = UEngineInput::GetInst().IsDown('A');
	bool isDownUp = UEngineInput::GetInst().IsDown(VK_UP);
	bool isDownDown = UEngineInput::GetInst().IsDown(VK_DOWN);

	if (isPainterMovingUp || isPainterMovingDown)
	{
		RunPaintSequence(_deltaTime);
		return;
	}

	if (isWaitToSelect)
	{
		RunWaitSequence(_deltaTime, SceneType::SELECT);
	}
	else if (isWaitToStart)
	{
		RunWaitSequence(_deltaTime, SceneType::START);
	}

	if (isDownF1 && Coin > 0)
	{
		isWaitToSelect = true;

		ResetSeconds();
		SwitchSelectUi(true);

		SRSelectSayHi->ChangeAnimation("SayHi");
	}
	else if (isDownF3 || isDownF4)
	{
		unsigned __int8 coin = (isDownF3 ? 1 : 2);
		ResetSeconds();
		AddCoin(coin);

		if (!isWaitToStart)
		{
			isWaitToStart = true;
			OnEndAnimation();
		}
	}
	else if (isDownA)
	{
		// TODO: state check
		if (isWaitToSelect)
		{
			// TODO: run circle anim
			SRSelectPainter->ChangeAnimation("DrawCircle", true);
		}
	}
	else if (isDownUp)
	{
		FVector2D curLoc = SRSelectPainter->GetComponentLocation();
		if (curLoc.Y >= PAINTER_END_LOC.Y)
		{
			SRSelectPainter->SetComponentLocation({ curLoc.X, curLoc.Y - 1 });
			isPainterMovingUp = true;
		}
	}
	else if (isDownDown)
	{
		FVector2D curLoc = SRSelectPainter->GetComponentLocation();
		if (curLoc.Y <= PAINTER_START_LOC.Y)
		{
			SRSelectPainter->SetComponentLocation({ curLoc.X, curLoc.Y + 1 });
			isPainterMovingDown = true;
		}
	}
}

void ATitle::AddCoin(unsigned __int8 _coinCnt)
{
	if (Coin >= 99) return;

	Coin += _coinCnt;

	int base10 = Coin / 10;
	int base1 = Coin % 10;

	SRCreditCount[0]->SetSprite(SPRITE_TIME_COUNT, base10);
	SRCreditCount[1]->SetSprite(SPRITE_TIME_COUNT, base1);
}

void ATitle::ResetSeconds()
{
	Seconds = START_SECONDS;
}

void ATitle::SwitchStartUi(bool _isShow)
{
	SRPushP1P2->SetActive(_isShow);
	SRTimeTitle->SetActive(_isShow);
	for (USpriteRenderer* renderer : SRTimeCount)
	{
		renderer->SetActive(_isShow);
	}
}

void ATitle::SwitchSelectUi(bool _isShow)
{
	SRSelectGameMode->SetActive(_isShow);
	for (int i = 0; i < 2; ++i)
	{
		SRSelectTimeCount[i]->SetActive(_isShow);
	}
	SRSelectSayHi->SetActive(_isShow);
	SRSelectPainter->SetActive(_isShow);
}

void ATitle::RunPaintSequence(float _deltaTime)
{
	FVector2D curLoc = SRSelectPainter->GetComponentLocation();
	static bool tempAnimFlag = true;

	if (isPainterMovingDown)
	{
		if (curLoc.Y <= PAINTER_END_LOC.Y)
		{
			if (tempAnimFlag)
			{
				SRSelectPainter->ChangeAnimation("GoingUpDown", true);
				tempAnimFlag = false;
			}
			SRSelectPainter->SetComponentLocation({ curLoc.X, curLoc.Y + 100 * _deltaTime });
		}
		else
		{
			tempAnimFlag = true;
			isPainterMovingUp = false;
			isPainterMovingDown = false;
		}
	}
	else if (isPainterMovingUp)
	{
		if (curLoc.Y >= PAINTER_START_LOC.Y)
		{
			if (tempAnimFlag)
			{
				SRSelectPainter->ChangeAnimation("GoingUpDown", true);
				tempAnimFlag = false;
			}
			SRSelectPainter->ChangeAnimation("GoingUpDown");
			SRSelectPainter->SetComponentLocation({ curLoc.X, curLoc.Y - 100 * _deltaTime });
		}
		else
		{
			tempAnimFlag = true;
			isPainterMovingUp = false;
			isPainterMovingDown = false;
		}
	}
}

void ATitle::RunWaitSequence(float _deltaTime, const SceneType& _type)
{
	static float elapsedSecs = 0.f;
	elapsedSecs += _deltaTime;

	if (elapsedSecs >= 1.f)
	{
		elapsedSecs = 0.f;
		Countdown(_type);
	}
}

void ATitle::Countdown(const SceneType& _type)
{
	if (Seconds < 0)
	{
		if (_type == SceneType::START)
		{
			// TODO: go to Select
		}
		else
		{
			// TODO: Start
		}
		return;
	}

	int base10 = Seconds / 10;
	int base1 = Seconds % 10;

	if (_type == SceneType::START)
	{
		SRTimeCount[0]->SetSprite(SPRITE_TIME_COUNT, base10);
		SRTimeCount[1]->SetSprite(SPRITE_TIME_COUNT, base1);
	}
	else
	{
		std::string resourcePath = SPRITE_SELECT_TIME_COUNT_B;
		if (base10 == 1)
		{
			resourcePath = SPRITE_SELECT_TIME_COUNT_Y;
		}
		else if (base10 == 0)
		{
			resourcePath = SPRITE_SELECT_TIME_COUNT_P;
		}
		SRSelectTimeCount[0]->SetSprite(resourcePath, base10);
		SRSelectTimeCount[1]->SetSprite(resourcePath, base1);
	}

	Seconds--;
}

void ATitle::OnEndAnimation()
{
	SROpening->ChangeAnimation(ANIM_IDLE_NAME);
	SwitchStartUi(true);
}

void ATitle::OnEndPainterDraw()
{
	SRSelectCircle->SetActive(true);
	SRSelectCircle->ChangeAnimation("DrawCircle");
}

void ATitle::OnEndCircleDraw()
{
	static bool tempFlag = true;
	if (tempFlag)
	{
		tempFlag = false;
		AFade::MainFade->FadeOut();
	}
}

void ATitle::OnEndFadeOut()
{
	UEngineAPICore::GetCore()->OpenLevel("Play");
}