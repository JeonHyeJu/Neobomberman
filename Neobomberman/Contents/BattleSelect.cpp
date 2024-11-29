#include "PreCompile.h"
#include "BattleSelect.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "Fade.h"
#include <EngineCore/SpriteRenderer.h>
#include <EnginePlatform/EngineInput.h>
#include <EnginePlatform/EngineSound.h>
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

	const int TP_FRAME_CNT = 7;
	std::vector<int> teleportReverseIdxs{ 9, 8, 7, 6, 5, 4, 3 };
	std::vector<float> teleportReverseSpeed(TP_FRAME_CNT, .1f);
	teleportReverseSpeed[TP_FRAME_CNT - 1] = .5f;
	const float TELEPORT_SPEED = .1f;

	Characters.reserve(4);	// Temp

	FVector2D firstLoc = FVector2D{ 64, 211 };
	SRCharacterWhite = CreateDefaultSubObject<USpriteRenderer>();
	SRCharacterWhite->SetComponentLocation(firstLoc);
	SRCharacterWhite->SetComponentScale(bombermanSize);
	SRCharacterWhite->SetSprite(SPRITE_CHARACTER_WHITE);
	SRCharacterWhite->SetOrder(ERenderOrder::PLAYER);
	SRCharacterWhite->CreateAnimation(ANIM_HEAD_UP, SPRITE_CHARACTER_WHITE, 0, 2, .2f, false);
	SRCharacterWhite->CreateAnimation(ANIM_HEAD_DOWN, SPRITE_CHARACTER_WHITE, reverseIdxs, times, false);
	SRCharacterWhite->CreateAnimation(ANIM_TELEPORT_FORWARD, SPRITE_CHARACTER_WHITE, 3, 9, TELEPORT_SPEED, false);
	SRCharacterWhite->CreateAnimation(ANIM_TELEPORT_BACKWARD, SPRITE_CHARACTER_WHITE, teleportReverseIdxs, teleportReverseSpeed, false);
	SRCharacterWhite->SetActive(false);
	Characters.push_back(SRCharacterWhite);

	SRCharacterBlack = CreateDefaultSubObject<USpriteRenderer>();
	SRCharacterBlack->SetComponentLocation(firstLoc + FVector2D{ STRIDE, 0 });
	SRCharacterBlack->SetComponentScale(bombermanSize);
	SRCharacterBlack->SetSprite(SPRITE_CHARACTER_BLACK);
	SRCharacterBlack->SetOrder(ERenderOrder::PLAYER);
	SRCharacterBlack->CreateAnimation(ANIM_HEAD_UP, SPRITE_CHARACTER_BLACK, 0, 2, .2f, false);
	SRCharacterBlack->CreateAnimation(ANIM_HEAD_DOWN, SPRITE_CHARACTER_BLACK, reverseIdxs, times, false);
	SRCharacterBlack->CreateAnimation(ANIM_TELEPORT_FORWARD, SPRITE_CHARACTER_BLACK, 3, 9, TELEPORT_SPEED, false);
	SRCharacterBlack->CreateAnimation(ANIM_TELEPORT_BACKWARD, SPRITE_CHARACTER_BLACK, teleportReverseIdxs, teleportReverseSpeed, false);
	SRCharacterBlack->SetActive(false);
	Characters.push_back(SRCharacterBlack);

	SRCharacterRed = CreateDefaultSubObject<USpriteRenderer>();
	SRCharacterRed->SetComponentLocation(firstLoc + FVector2D{ STRIDE*2, 0 });
	SRCharacterRed->SetComponentScale(bombermanSize);
	SRCharacterRed->SetSprite(SPRITE_CHARACTER_RED);
	SRCharacterRed->SetOrder(ERenderOrder::PLAYER);
	SRCharacterRed->CreateAnimation(ANIM_HEAD_UP, SPRITE_CHARACTER_RED, 0, 2, .2f, false);
	SRCharacterRed->CreateAnimation(ANIM_HEAD_DOWN, SPRITE_CHARACTER_RED, reverseIdxs, times, false);
	SRCharacterRed->CreateAnimation(ANIM_TELEPORT_FORWARD, SPRITE_CHARACTER_RED, 3, 9, TELEPORT_SPEED, false);
	SRCharacterRed->CreateAnimation(ANIM_TELEPORT_BACKWARD, SPRITE_CHARACTER_RED, teleportReverseIdxs, teleportReverseSpeed, false);
	SRCharacterRed->SetActive(false);
	Characters.push_back(SRCharacterRed);

	SRCharacterBlue = CreateDefaultSubObject<USpriteRenderer>();
	SRCharacterBlue->SetComponentLocation(firstLoc + FVector2D{ STRIDE * 3, 0 });
	SRCharacterBlue->SetComponentScale(bombermanSize);
	SRCharacterBlue->SetSprite(SPRITE_CHARACTER_BLUE);
	SRCharacterBlue->SetOrder(ERenderOrder::PLAYER);
	SRCharacterBlue->CreateAnimation(ANIM_HEAD_UP, SPRITE_CHARACTER_BLUE, 0, 2, .2f, false);
	SRCharacterBlue->CreateAnimation(ANIM_HEAD_DOWN, SPRITE_CHARACTER_BLUE, reverseIdxs, times, false);
	SRCharacterBlue->CreateAnimation(ANIM_TELEPORT_FORWARD, SPRITE_CHARACTER_BLUE, 3, 9, TELEPORT_SPEED, false);
	SRCharacterBlue->CreateAnimation(ANIM_TELEPORT_BACKWARD, SPRITE_CHARACTER_BLUE, teleportReverseIdxs, teleportReverseSpeed, false);
	SRCharacterBlue->SetActive(false);
	Characters.push_back(SRCharacterBlue);

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

	{
		FVector2D size{ 128, 16 };
		SRPlayer1Name = CreateDefaultSubObject<USpriteRenderer>();
		SRPlayer1Name->SetComponentLocation(FVector2D{ 120, 375 });
		SRPlayer1Name->SetComponentScale(size);
		SRPlayer1Name->SetSprite(SPRITE_CHARACTER_NAMES);
		SRPlayer1Name->SetOrder(ERenderOrder::UI);
		SRPlayer1Name->SetActive(false);
	}

	{
		FVector2D size{ 64, 64 };
		SRPlayer1Sign = CreateDefaultSubObject<USpriteRenderer>();
		SRPlayer1Sign->SetComponentScale(size);
		SRPlayer1Sign->SetSprite(SPRITE_SIGN_1P);
		SRPlayer1Sign->SetOrder(ERenderOrder::UI);
		SRPlayer1Sign->SetActive(false);
	}

	Fsm.CreateState(ESceneState::SELECT_MAP, std::bind(&ABattleSelect::SelectingMap, this, std::placeholders::_1));
	Fsm.CreateState(ESceneState::WAITING_MAP_FADE, nullptr);
	Fsm.CreateState(ESceneState::SELECT_CHARACTER, std::bind(&ABattleSelect::SelectingCharacter, this, std::placeholders::_1), std::bind(&ABattleSelect::OnSelectCharacter, this));
	Fsm.CreateState(ESceneState::WAITING_CHARACTER_FADE, nullptr);
	Fsm.CreateState(ESceneState::COMPLETE_SELECT, std::bind(&ABattleSelect::CompletingSelect, this, std::placeholders::_1));

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
		UEngineSound::AllSoundStop();
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
	SRPlayer1Name->SetActive(true);

	SRCharacterWhite->ChangeAnimation(ANIM_HEAD_UP);
}

void ABattleSelect::SelectingMap(float _deltaTime)
{
	static bool isOnce = true;
	if (UEngineInput::GetInst().IsDown(VK_SPACE) || UEngineInput::GetInst().IsDown(VK_RETURN))
	{
		if (isOnce)
		{
			isOnce = false;
			UEngineSound::Play(SFXMenuSelect);
			ChangeToSelectCharacter();
			return;
		}
	}
}

void ABattleSelect::SelectingCharacter(float _deltaTime)
{
	if (UEngineInput::GetInst().IsDown(VK_SPACE) || UEngineInput::GetInst().IsDown(VK_RETURN))
	{
		UEngineSound::Play(SFXMenuSelect);
		SRPlayer1Sign->SetComponentLocation(SRBalloonBBM->GetComponentLocation() + FVector2D{ -28, 21 });
		SRPlayer1Sign->SetActive(true);

		Characters[CurIdx]->ChangeAnimation(ANIM_TELEPORT_FORWARD);
		Fsm.ChangeState(ESceneState::COMPLETE_SELECT);
		return;
	}

	FVector2D direction;

	bool isUp = UEngineInput::GetInst().IsDown(VK_UP);
	bool isDown = UEngineInput::GetInst().IsDown(VK_DOWN);
	bool isLeft = UEngineInput::GetInst().IsDown(VK_LEFT);
	bool isRight = UEngineInput::GetInst().IsDown(VK_RIGHT);

	if (!(isUp || isDown || isLeft || isRight)) return;

	if (isUp)
	{
		direction = FVector2D::UP;
	}
	else if (isDown)
	{
		direction = FVector2D::DOWN;
	}
	else if (isLeft)
	{
		direction = FVector2D::LEFT;
	}
	else if (isRight)
	{
		direction = FVector2D::RIGHT;
	}

	// TODO: limit
	FVector2D addedPos = direction* static_cast<float>(STRIDE);
	FVector2D nextPos = SRBalloonBBM->GetComponentLocation() + addedPos;
	if (nextPos.X < 0 || nextPos.X > 380)	// Temp
	{
		return;
	}
	if (nextPos.Y < 170 || nextPos.Y > 180)
	{
		return;
	}

	PreIdx = CurIdx;
	CurIdx += static_cast<int>(direction.X);
	SRPlayer1Name->SetSprite(SPRITE_CHARACTER_NAMES, CurIdx);
	
	Characters[CurIdx]->ChangeAnimation(ANIM_HEAD_UP);
	Characters[PreIdx]->ChangeAnimation(ANIM_HEAD_DOWN);

	//OutputDebugString(("CurIdx : " + std::to_string(CurIdx) + "\n").c_str());
	//OutputDebugString(("nextPos: " + std::to_string(nextPos.X) + ", " + std::to_string(nextPos.Y) + "\n").c_str());
	SRBalloonBBM->AddComponentLocation(addedPos);
	UEngineSound::Play(SFXMoveBBM, -1, 0, false);
}


void ABattleSelect::CompletingSelect(float _deltaTime)
{
	if (Characters[CurIdx]->IsCurAnimationEnd())
	{
		if (Characters[CurIdx]->GetCurAnimName() == ANIM_TELEPORT_FORWARD)
		{
			Characters[CurIdx]->SetComponentLocation(FVector2D{ 256, 392 });
			Characters[CurIdx]->ChangeAnimation(ANIM_TELEPORT_BACKWARD);
		}
		else  // ANIM_TELEPORT_BACKWARD
		{
			ChangeToBattleScene();
		}
	}
}