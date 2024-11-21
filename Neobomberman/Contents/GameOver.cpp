#include "PreCompile.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "GameOver.h"
#include "Fade.h"
#include <EngineCore/SpriteRenderer.h>

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
		const char* spritePath = "ContinueNumber.png";
		FVector2D size = GlobalVar::BOMBERMAN_SIZE;

		SRContinueNum = CreateDefaultSubObject<USpriteRenderer>();
		SRContinueNum->SetSprite(spritePath, 0);
		SRContinueNum->SetComponentLocation(winSize.Half() + FVector2D{ 0.f, size.Y + 10.f });
		SRContinueNum->SetComponentScale(size);
		SRContinueNum->SetOrder(ERenderOrder::UI_OVER_FADE);

		SRContinueNum->CreateAnimation("Run9", spritePath, 0, 2, 0.33f, false);
		SRContinueNum->CreateAnimation("Run8", spritePath, 3, 5, 0.33f, false);
		SRContinueNum->CreateAnimation("Run7", spritePath, 6, 8, 0.33f, false);
		SRContinueNum->CreateAnimation("Run6", spritePath, 9, 11, 0.33f, false);
		SRContinueNum->CreateAnimation("Run5", spritePath, 12, 14, 0.33f, false);
		SRContinueNum->CreateAnimation("Run4", spritePath, 15, 17, 0.33f, false);
		SRContinueNum->CreateAnimation("Run3", spritePath, 18, 20, 0.33f, false);
		SRContinueNum->CreateAnimation("Run2", spritePath, 21, 23, 0.33f, false);
		SRContinueNum->CreateAnimation("Run1", spritePath, 24, 26, 0.33f, false);
		SRContinueNum->CreateAnimation("Run0", spritePath, 27, 29, 0.33f, false);
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
}

void AGameOver::Reset()
{
	ElapsedSecs = 0.f;
	CurSeconds = WAIT_SECONDS;
	IsOver = false;

	SRContinueStr->ChangeAnimation("Run", true);
	SRContinueNum->ChangeAnimation("Run9", true);
}

void AGameOver::Countdown(float _deltaTime)
{
	ElapsedSecs += _deltaTime;

	if (ElapsedSecs >= 1.f)
	{
		ElapsedSecs = 0.f;
		CurSeconds -= 1;

		if (CurSeconds == 0)
		{
			GameOver();
			return;
		}

		SRContinueNum->ChangeAnimation("Run"+std::to_string(CurSeconds - 1));
	}
}

void AGameOver::GameOver()
{
	if (!IsOver)
	{
		IsOver = true;
		SetActive(false);
	}
}