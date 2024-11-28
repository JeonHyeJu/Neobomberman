#include "PreCompile.h"
#include "BattleSelect.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "Fade.h"
#include <EngineCore/SpriteRenderer.h>
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/EngineAPICore.h>

ABattleSelect::ABattleSelect()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	SRBackground = CreateDefaultSubObject<USpriteRenderer>();
	SRBackground->SetComponentLocation(winSize.Half());
	SRBackground->SetComponentScale(winSize);
	SRBackground->SetSprite(SPRITE_SELECT_MAP);
	SRBackground->SetOrder(ERenderOrder::BACKGROUND);

	SRSirenL = CreateDefaultSubObject<USpriteRenderer>();
	SRSirenL->SetComponentLocation(FVector2D{ 180, 170 });
	SRSirenL->SetComponentScale(FVector2D{ 32, 32 });
	SRSirenL->SetSprite("Siren.png");
	SRSirenL->SetOrder(ERenderOrder::BACKGROUND_OVER);
	SRSirenL->CreateAnimation("Alert", "Siren.png", 0, 3, .2f);

	SRSirenR = CreateDefaultSubObject<USpriteRenderer>();
	SRSirenR->SetComponentLocation(FVector2D{ 428, 170 });
	SRSirenR->SetComponentScale(FVector2D{ 32, 32 });
	SRSirenR->SetSprite("Siren.png");
	SRSirenR->SetOrder(ERenderOrder::BACKGROUND_OVER);
	SRSirenR->CreateAnimation("Alert", "Siren.png", 0, 3, .2f);

	SRSirenL->ChangeAnimation("Alert");
	SRSirenR->ChangeAnimation("Alert");

	for (int i = 0; i < 2; ++i)
	{
		FVector2D size = { 24, 48 };
		SRSelectTimeCount[i] = CreateDefaultSubObject<USpriteRenderer>();
		SRSelectTimeCount[i]->SetOrder(ERenderOrder::OPENING_STRING);
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

	FVector2D bombermanSize = GlobalVar::BOMBERMAN_SIZE;
	std::vector<int> reverseIdxs{ 2, 1, 0 };
	std::vector<float> times(3, .2f);

	SRCharacterWhite = CreateDefaultSubObject<USpriteRenderer>();
	SRCharacterWhite->SetComponentLocation(bombermanSize.Half());
	SRCharacterWhite->SetComponentScale(bombermanSize);
	SRCharacterWhite->SetSprite(SPRITE_CHARACTER_WHITE);
	SRCharacterWhite->SetOrder(ERenderOrder::PLAYER);
	SRCharacterWhite->CreateAnimation("HeadUp", SPRITE_CHARACTER_WHITE, 0, 2, .2f, false);
	SRCharacterWhite->CreateAnimation("HeadDown", SPRITE_CHARACTER_WHITE, reverseIdxs, times, false);
	SRCharacterWhite->SetActive(false);

	SRCharacterBlack = CreateDefaultSubObject<USpriteRenderer>();
	SRCharacterBlack->SetComponentLocation(bombermanSize.Half());
	SRCharacterBlack->SetComponentScale(bombermanSize);
	SRCharacterBlack->SetSprite(SPRITE_CHARACTER_BLACK);
	SRCharacterBlack->SetOrder(ERenderOrder::PLAYER);
	SRCharacterBlack->CreateAnimation("HeadUp", SPRITE_CHARACTER_BLACK, 0, 2, .2f, false);
	SRCharacterBlack->CreateAnimation("HeadDown", SPRITE_CHARACTER_WHITE, reverseIdxs, times, false);
	SRCharacterBlack->SetActive(false);

	SRCharacterRed = CreateDefaultSubObject<USpriteRenderer>();
	SRCharacterRed->SetComponentLocation(bombermanSize.Half());
	SRCharacterRed->SetComponentScale(bombermanSize);
	SRCharacterRed->SetSprite(SPRITE_CHARACTER_RED);
	SRCharacterRed->SetOrder(ERenderOrder::PLAYER);
	SRCharacterRed->CreateAnimation("HeadUp", SPRITE_CHARACTER_RED, 0, 2, .2f, false);
	SRCharacterRed->CreateAnimation("HeadDown", SPRITE_CHARACTER_WHITE, reverseIdxs, times, false);
	SRCharacterRed->SetActive(false);

	SRCharacterBlue = CreateDefaultSubObject<USpriteRenderer>();
	SRCharacterBlue->SetComponentLocation(bombermanSize.Half());
	SRCharacterBlue->SetComponentScale(bombermanSize);
	SRCharacterBlue->SetSprite(SPRITE_CHARACTER_BLUE);
	SRCharacterBlue->SetOrder(ERenderOrder::PLAYER);
	SRCharacterBlue->CreateAnimation("HeadUp", SPRITE_CHARACTER_BLUE, 0, 2, .2f, false);
	SRCharacterBlue->CreateAnimation("HeadDown", SPRITE_CHARACTER_WHITE, reverseIdxs, times, false);
	SRCharacterBlue->SetActive(false);

	{
		FVector2D size{ 156, 156 };
		SRBalloonBBM = CreateDefaultSubObject<USpriteRenderer>();
		SRBalloonBBM->SetComponentLocation(FVector2D{ 91, 174 });
		SRBalloonBBM->SetComponentScale(size);
		SRBalloonBBM->SetSprite(SPRITE_BALLOON_BBM);
		SRBalloonBBM->SetOrder(ERenderOrder::PLAYER);
		SRBalloonBBM->CreateAnimation("Fly", SPRITE_BALLOON_BBM, 0, 3, .2f);
		SRBalloonBBM->ChangeAnimation("Fly");
		SRBalloonBBM->SetActive(false);
	}

	Fsm.CreateState(ESceneState::SELECT_MAP, std::bind(&ABattleSelect::SelectingMap, this, std::placeholders::_1));
	Fsm.CreateState(ESceneState::WAITING_MAP_FADE, nullptr);
	Fsm.CreateState(ESceneState::SELECT_CHARACTER, std::bind(&ABattleSelect::SelectingCharacter, this, std::placeholders::_1), std::bind(&ABattleSelect::OnSelectCharacter, this));
	Fsm.CreateState(ESceneState::WAITING_CHARACTER_FADE, nullptr);

	Fsm.ChangeState(ESceneState::SELECT_MAP);
}

ABattleSelect::~ABattleSelect()
{
}

void ABattleSelect::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleSelect::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	Fsm.Update(_deltaTime);

	static float elpasedSecs = 0.f;
	elpasedSecs += _deltaTime;
	if (elpasedSecs >= 1.f)
	{
		elpasedSecs = 0.f;

		CountDown();
	}
}

void ABattleSelect::ResetSeconds()
{
	Seconds = START_SECONDS;
}

void ABattleSelect::CountDown()
{
	if (Seconds < 0)
	{
		if (static_cast<ESceneState>(Fsm.GetState()) == ESceneState::SELECT_MAP)
		{
			ChangeToSelectCharacter();
			return;
		}
		else
		{
			ChangeToBattleScene();
		}
	}

	int base10 = Seconds / 10;
	int base1 = Seconds % 10;

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

	Seconds--;
}

void ABattleSelect::ChangeToSelectCharacter()
{
	SRSirenL->SetActive(false);
	SRSirenR->SetActive(false);

	AFade::MainFade->BindEndEvent(std::bind(&ABattleSelect::OnEndFadeOut, this));
	AFade::MainFade->FadeOut();
	ResetSeconds();

	Fsm.ChangeState(ESceneState::WAITING_MAP_FADE);
}

void ABattleSelect::ChangeToBattleScene()
{
	AFade::MainFade->BindEndEvent(std::bind(&ABattleSelect::OnEndFadeOut, this));
	AFade::MainFade->FadeOut();
	ResetSeconds();

	Fsm.ChangeState(ESceneState::WAITING_CHARACTER_FADE);
}

void ABattleSelect::OnEndFadeOut()
{
	ESceneState curState = static_cast<ESceneState>(Fsm.GetState());
	switch (curState)
	{
	case ABattleSelect::ESceneState::WAITING_MAP_FADE:
		AFade::MainFade->FadeIn();
		Fsm.ChangeState(ESceneState::SELECT_CHARACTER);
		break;
	case ABattleSelect::ESceneState::WAITING_CHARACTER_FADE:
		AFade::MainFade->FadeIn();
		UEngineAPICore::GetCore()->OpenLevel("BattlePlay");
		break;
	default:
		break;
	}
}

void ABattleSelect::OnSelectCharacter()
{
	SRBackground->SetSprite(SPRITE_SELECT_CHARACTER);

	SRBalloonBBM->SetActive(true);
	SRCharacterWhite->SetActive(true);
	SRCharacterBlack->SetActive(true);
	SRCharacterRed->SetActive(true);
	SRCharacterBlue->SetActive(true);
}

void ABattleSelect::SelectingMap(float _deltaTime)
{
	static bool isOnce = true;
	if (UEngineInput::GetInst().IsDown(VK_SPACE) || UEngineInput::GetInst().IsDown(VK_RETURN))
	{
		if (isOnce)
		{
			isOnce = false;
			ChangeToSelectCharacter();
			return;
		}
	}
}

void ABattleSelect::SelectingCharacter(float _deltaTime)
{
}
