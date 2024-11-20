#include "PreCompile.h"
#include "Title.h"
#include "ContentsEnum.h"
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
		SRLevel_4->SetOrder(ERenderOrder::UI_OVER_FADE);
	}

	{
		FVector2D size = { 112, 16 };
		SRCredits = CreateDefaultSubObject<USpriteRenderer>();
		SRCredits->SetSprite("Credits_112x16.png");
		SRCredits->SetComponentScale(size);
		SRCredits->SetComponentLocation({ 432 + size.hX(), winSize.Y - size.hY()});
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

	{
		FVector2D size = { 608, 256 };
		SRCutSceneBg = CreateDefaultSubObject<USpriteRenderer>();
		SRCutSceneBg->SetSprite("CutSceneStage1");
		SRCutSceneBg->SetComponentLocation({ size.hX(), 132.f });
		SRCutSceneBg->SetComponentScale(size);

		const int FRAME_CNT = 9;
		std::vector<int> idxs;
		std::vector<float> frames(FRAME_CNT , .3f);
		idxs.reserve(FRAME_CNT);
		frames.reserve(FRAME_CNT);

		for (int i = 0; i < FRAME_CNT; ++i)
		{
			idxs.push_back(i);
		}
		frames[1] = .01f;
		frames[FRAME_CNT-1] = 3.f;

		SRCutSceneBg->CreateAnimation("Wait", "CutSceneStage1", 0, 0, 1.f);
		SRCutSceneBg->CreateAnimation("Dark", "CutSceneStage1", idxs, frames, false);
		SRCutSceneBg->SetAnimationEvent("Dark", FRAME_CNT-1, std::bind(&ATitle::OnEndCutScene, this));

		SRCutSceneBg->ChangeAnimation("Wait");
		SRCutSceneBg->SetOrder(ERenderOrder::CUT_SCENE_BG);
	}

	{
		FVector2D size = { 608, 306 };
		SRCutScene = CreateDefaultSubObject<USpriteRenderer>();
		SRCutScene->SetSprite("CutSceneFront.png");
		SRCutScene->SetComponentLocation({ size.hX(), 220.f });
		SRCutScene->SetComponentScale(size);

		SRCutScene->SetOrder(ERenderOrder::CUT_SCENE);
	}

	{
		FVector2D size = { 256, 256 };
		SRSpaceship = CreateDefaultSubObject<USpriteRenderer>();
		SRSpaceship->SetSprite("Spaceship.png");
		SRSpaceship->SetComponentLocation({ winSize.hX(), winSize.hY() * 1.5f });
		SRSpaceship->SetComponentScale(size);

		SRSpaceship->SetOrder(ERenderOrder::CUT_SPACESHIP);
	}

	{
		FVector2D size = { 608, 224 };
		SRLetterBoxUpper = CreateDefaultSubObject<USpriteRenderer>();
		SRLetterBoxUpper->SetSprite("Letterbox.png");
		SRLetterBoxUpper->SetComponentLocation({ winSize.hX(), size.hY() });
		SRLetterBoxUpper->SetComponentScale(size);
		SRLetterBoxUpper->SetOrder(ERenderOrder::CUT_LETTER_BOX);
		SRLetterBoxUpper->SetActive(false);

		SRLetterBoxLower = CreateDefaultSubObject<USpriteRenderer>();
		SRLetterBoxLower->SetSprite("Letterbox.png");
		SRLetterBoxLower->SetComponentLocation({ winSize.hX(), size.Y + size.hY() });
		SRLetterBoxLower->SetComponentScale(size);
		SRLetterBoxLower->SetOrder(ERenderOrder::CUT_LETTER_BOX);
		SRLetterBoxLower->SetActive(false);
	}

	SwitchStartUi(false);
	SwitchSelectUi(false);
	SwitchCutSceneUi(false);

	FSM.CreateState(ETitleState::OPENING, nullptr, std::bind(&ATitle::OnRunOpening, this));
	FSM.CreateState(ETitleState::WAIT_START, std::bind(&ATitle::WaitingToStart, this, std::placeholders::_1), std::bind(&ATitle::OnWaitToStart, this));
	FSM.CreateState(ETitleState::WAIT_SELECT_IDLE, nullptr);
	FSM.CreateState(ETitleState::WAIT_SELECT, std::bind(&ATitle::SelectingMode, this, std::placeholders::_1), std::bind(&ATitle::OnSelectMode, this));
	FSM.CreateState(ETitleState::RUN_CUT_SCENE_IDLE, nullptr);
	FSM.CreateState(ETitleState::RUN_CUT_SCENE, std::bind(&ATitle::RunningCutScene, this, std::placeholders::_1), std::bind(&ATitle::OnRunCutScene, this));
	FSM.CreateState(ETitleState::PREPARE_PLAY, nullptr);
	FSM.CreateState(ETitleState::PREPARE_DISAPPEAR, nullptr);
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

	FSM.ChangeState(ETitleState::OPENING);
}

void ATitle::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	CoinAction();

	if (isPainterMovingUp || isPainterMovingDown)
	{
		RunPaintSequence(_deltaTime);
		return;
	}

	FSM.Update(_deltaTime);
}

void ATitle::AddCoin(int _coinCnt)
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

void ATitle::SwitchCutSceneUi(bool _isShow)
{
	SRCutSceneBg->SetActive(_isShow);
	SRCutScene->SetActive(_isShow);
	SRSpaceship->SetActive(_isShow);
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

void ATitle::RunWaitSequence(float _deltaTime, const ESceneType& _type)
{
	static float elapsedSecs = 0.f;
	elapsedSecs += _deltaTime;

	if (elapsedSecs >= 1.f)
	{
		elapsedSecs = 0.f;
		Countdown(_type);
	}
}

void ATitle::Countdown(const ESceneType& _type)
{
	if (Seconds < 0)
	{
		if (_type == ESceneType::START)
		{
			// TODO: Play video
			// Temp
			if (Coin == 0) Coin++;
			ChangeToStartScene();
		}
		else
		{
			ChangeToCutScene();
		}
		return;
	}

	int base10 = Seconds / 10;
	int base1 = Seconds % 10;

	if (_type == ESceneType::START)
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

void ATitle::ChangeToCutScene()
{
	if (FSM.GetState() < static_cast<int>(ETitleState::RUN_CUT_SCENE_IDLE))
	{
		FSM.ChangeState(ETitleState::RUN_CUT_SCENE_IDLE);
		SRSelectPainter->ChangeAnimation("DrawCircle", true);
		// The following actions are executed in an animated end event.
		return;
	}
}

void ATitle::ChangeToStartScene()
{
	if (FSM.GetState() < static_cast<int>(ETitleState::WAIT_SELECT_IDLE))
	{
		AddCoin(-1);
		AFade::MainFade->FadeOut();
		FSM.ChangeState(ETitleState::WAIT_SELECT_IDLE);
	}
}

void ATitle::CoinAction()
{
	bool isDownF3 = UEngineInput::GetInst().IsDown(VK_F3);
	bool isDownF4 = UEngineInput::GetInst().IsDown(VK_F4);

	if (isDownF3 || isDownF4)
	{
		int coin = (isDownF3 ? 1 : 2);
		if (FSM.GetState() <= static_cast<int>(ETitleState::WAIT_START))
		{
			ResetSeconds();
		}
		AddCoin(coin);
		OnEndAnimation();
	}
}

void ATitle::OnEndAnimation()
{
	if (FSM.GetState() < static_cast<int>(ETitleState::WAIT_START))
	{
		FSM.ChangeState(ETitleState::WAIT_START);
	}
}

void ATitle::OnEndPainterDraw()
{
	SRSelectCircle->SetActive(true);
	SRSelectCircle->ChangeAnimation("DrawCircle");
}

void ATitle::OnEndCircleDraw()
{
	AFade::MainFade->FadeOut();
}

void ATitle::OnEndFadeOut()
{
	ETitleState state = static_cast<ETitleState>(FSM.GetState());

	switch (state)
	{
	case ETitleState::WAIT_SELECT_IDLE:
		FSM.ChangeState(ETitleState::WAIT_SELECT);
		AFade::MainFade->FadeIn();
		break;
	case ETitleState::RUN_CUT_SCENE_IDLE:
		FSM.ChangeState(ETitleState::RUN_CUT_SCENE);
		AFade::MainFade->FadeIn();
		break;
	case ETitleState::PREPARE_PLAY:
		FSM.ChangeState(ETitleState::PREPARE_DISAPPEAR);
		UEngineAPICore::GetCore()->OpenLevel("Play");
	default:
		break;
	}
}

void ATitle::OnRunOpening()
{
	SROpening->ChangeAnimation(ANIM_RUN_NAME);
}

void ATitle::OnEndCutScene()
{
	if (FSM.GetState() < static_cast<int>(ETitleState::PREPARE_PLAY))
	{
		AFade::MainFade->FadeOut();
		FSM.ChangeState(ETitleState::PREPARE_PLAY);
	}
}

/* FSM */
void ATitle::OnWaitToStart()
{
	ResetSeconds();
	SROpening->ChangeAnimation(ANIM_IDLE_NAME);
	SwitchStartUi(true);
}

void ATitle::OnSelectMode()
{
	ResetSeconds();
	SROpening->SetActive(false);
	SwitchStartUi(false);
	SwitchSelectUi(true);
	SRSelectSayHi->ChangeAnimation("SayHi");
}

void ATitle::OnRunCutScene()
{
	SRSelectCircle->SetActive(false);
	SRLetterBoxUpper->SetActive(true);
	SRLetterBoxLower->SetActive(true);

	SwitchStartUi(false);
	SwitchSelectUi(false);
	SwitchCutSceneUi(true);
}

void ATitle::WaitingToStart(float _deltaTime)
{
	RunWaitSequence(_deltaTime, ESceneType::START);

	bool isDownF1 = UEngineInput::GetInst().IsDown(VK_F1);
	if (isDownF1)
	{
		ChangeToStartScene();
	}
}

void ATitle::SelectingMode(float _deltaTime)
{
	RunWaitSequence(_deltaTime, ESceneType::SELECT);

	bool isDownA = UEngineInput::GetInst().IsDown('A');
	if (isDownA)
	{
		if (FSM.GetState() < static_cast<int>(ETitleState::RUN_CUT_SCENE_IDLE))
		{
			ChangeToCutScene();
		}
	}

	bool isDownUp = UEngineInput::GetInst().IsDown(VK_UP);
	bool isDownDown = UEngineInput::GetInst().IsDown(VK_DOWN);

	if (isDownUp)
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

void ATitle::RunningCutScene(float _deltaTime)
{
	static float elapsedTime = 0.f;
	elapsedTime += _deltaTime;

	// TODO: Don't use time
	if (elapsedTime > 1.f && elapsedTime <= 3.f)
	{
		float speed = 200.f;
		FVector2D loc = SRLetterBoxUpper->GetComponentLocation();

		if (loc.Y > -20)	// 112(size.hY) - 96
		{
			SRLetterBoxUpper->SetComponentLocation(loc - FVector2D{ 0.f, 1.f * speed * _deltaTime });
			SRLetterBoxLower->SetComponentLocation(SRLetterBoxLower->GetComponentLocation() + FVector2D{0.f, 1.f * speed * _deltaTime});
		}
	}
	else if (elapsedTime > 4.f)
	{
		if (SRCutSceneBg->GetCurAnimName() != "Dark")
		{
			SRCutSceneBg->ChangeAnimation("Dark");
		}
	}
	else if (elapsedTime > 3.f)
	{
		float speed = 200.f;
		FVector2D loc = SRSpaceship->GetComponentLocation();

		// Temp
		if (loc.Y > 170)
		{
			SRSpaceship->SetComponentLocation(loc - FVector2D{ 0.f, 1.f * speed * _deltaTime });
		}
	}

	if (UEngineInput::GetInst().IsDown('A'))
	{
		OnEndCutScene();
	}
}