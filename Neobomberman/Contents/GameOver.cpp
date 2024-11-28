#include "PreCompile.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "GameOver.h"
#include "Fade.h"
#include <EngineCore/SpriteRenderer.h>
#include <EnginePlatform/EngineSound.h>

AGameOver::AGameOver()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	{
		const char* spritePath = "Continue.png";
		FVector2D size = { 534, 64 };

		SRContinueStr = CreateDefaultSubObject<USpriteRenderer>();
		SRContinueStr->SetSprite(spritePath, 0);
		SRContinueStr->SetComponentLocation(winSize.Half());
		SRContinueStr->SetComponentScale(size);
		SRContinueStr->SetOrder(ERenderOrder::UI_OVER_FADE);
		SRContinueStr->CreateAnimation("Run", spritePath, 0, 10, 0.5f, true);
	}

	{
		const char* SPRITE_PATH = "ContinueNumber.png";
		FVector2D size = GlobalVar::BOMBERMAN_SIZE;

		SRContinueNum = CreateDefaultSubObject<USpriteRenderer>();
		SRContinueNum->SetSprite(SPRITE_PATH, 0);
		SRContinueNum->SetComponentLocation(winSize.Half() + FVector2D{ 0.f, size.Y + 10.f });
		SRContinueNum->SetComponentScale(size);
		SRContinueNum->SetOrder(ERenderOrder::UI_OVER_FADE);

		SRContinueNum->CreateAnimation("Run9", SPRITE_PATH, 0, 2, 0.33f, false);
		SRContinueNum->CreateAnimation("Run8", SPRITE_PATH, 3, 5, 0.33f, false);
		SRContinueNum->CreateAnimation("Run7", SPRITE_PATH, 6, 8, 0.33f, false);
		SRContinueNum->CreateAnimation("Run6", SPRITE_PATH, 9, 11, 0.33f, false);
		SRContinueNum->CreateAnimation("Run5", SPRITE_PATH, 12, 14, 0.33f, false);
		SRContinueNum->CreateAnimation("Run4", SPRITE_PATH, 15, 17, 0.33f, false);
		SRContinueNum->CreateAnimation("Run3", SPRITE_PATH, 18, 20, 0.33f, false);
		SRContinueNum->CreateAnimation("Run2", SPRITE_PATH, 21, 23, 0.33f, false);
		SRContinueNum->CreateAnimation("Run1", SPRITE_PATH, 24, 26, 0.33f, false);
		SRContinueNum->CreateAnimation("Run0", SPRITE_PATH, 27, 29, 0.33f, false);
	}

	{
		FVector2D size = GlobalVar::BOMBERMAN_SIZE;
		int vecSize = sizeof(SRGameOverStrs) / sizeof(USpriteRenderer*);
		const int STRIDE = 5;
		int margin = 120;

		for (int i = 0; i < vecSize; ++i)
		{
			if (i > 3) margin = 140;

			int curIdx = i * STRIDE;
			SRGameOverStrs[i] = CreateDefaultSubObject<USpriteRenderer>();
			SRGameOverStrs[i]->SetSprite(GAMEOVER_PATH, curIdx);
			SRGameOverStrs[i]->SetComponentLocation({ margin + i * size.X * 0.85f, winSize.hY()});
			SRGameOverStrs[i]->SetComponentScale(size);
			SRGameOverStrs[i]->SetOrder(ERenderOrder::UI_OVER_FADE);
			SRGameOverStrs[i]->SetActive(false);
		}
	}
}

AGameOver::~AGameOver()
{
}

void AGameOver::BeginPlay()
{
	Super::BeginPlay();
}

void AGameOver::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	if (!IsOver)
	{
		Countdown(_deltaTime);
	}
}

void AGameOver::ShowAndStart()
{
	Reset();
	SetActive(true);
	UEngineSound::Play(SFXContinue);
}

void AGameOver::Reset()
{
	UEngineSound::StopPlayer(SFXContinue);

	ElapsedSecs = 0.f;
	CurSeconds = WAIT_SECONDS;
	TIME_CNT = .9f;
	IsOver = false;
	GameoverSeconds = 0;

	SRContinueStr->ChangeAnimation("Run", true);
	SRContinueNum->ChangeAnimation("Run9", true);
}

void AGameOver::Countdown(float _deltaTime)
{
	ElapsedSecs += _deltaTime;

	if (ElapsedSecs >= TIME_CNT)
	{
		ElapsedSecs = 0.f;
		CurSeconds -= 1;

		if (CurSeconds <= 0)
		{
			if (SRContinueNum->IsCurAnimationEnd())
			{
				TIME_CNT = .4f;		// Temp
				SRContinueStr->SetActive(false);
				SRContinueNum->SetActive(false);

				if (GameoverSeconds > 12)	// Temp
				{
					GameOver();
					return;
				}

				if (GameoverSeconds == 0)
				{
					UEngineSound::StopPlayer(SFXContinue);
					UEngineSound::Play(SFXGameOver);
				}

				GameOverStrAnim();
			}
		}
		else
		{
			UEngineSound::Play(SFXBonusCount);
			SRContinueNum->ChangeAnimation("Run" + std::to_string(CurSeconds - 1));
		}
	}
}

void AGameOver::GameOverStrAnim()
{
	int vecSize = sizeof(SRGameOverStrs) / sizeof(USpriteRenderer*);
	const int STRIDE = 5;

	for (int i = 0; i < vecSize; ++i)
	{
		USpriteRenderer* curSprite = SRGameOverStrs[i];

		if (GameoverSeconds == i)
		{
			if (curSprite == nullptr) return;

			curSprite->SetActive(true);
			continue;
		}

		if (curSprite->IsActive())
		{
			int addSec = GameoverSeconds - i;
			int clampped = i * STRIDE + (addSec > 4 ? 4 : addSec);
			SRGameOverStrs[i]->SetSprite(GAMEOVER_PATH, clampped);
		}
	}

	GameoverSeconds++;
}

void AGameOver::GameOver()
{
	if (!IsOver)
	{
		IsOver = true;
		SetActive(false);
	}
}