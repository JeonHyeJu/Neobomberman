#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "GameData.h"
#include "GameUI.h"
#include <EngineCore/SpriteRenderer.h>
#include <EnginePlatform/EngineInput.h>

AGameUI::AGameUI()
: AActor()
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

AGameUI::~AGameUI()
{

}

void AGameUI::BeginPlay()
{
	Super::BeginPlay();
	SetCoinSprite();
}

void AGameUI::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	CheckCoin();
}

unsigned __int8 AGameUI::GetCoin() const
{
	return GameData::GetInstance().GetCoin();
}

void AGameUI::AddCoin(unsigned __int8 __addVal)
{
	unsigned __int8 coin = GameData::GetInstance().AddCoin(__addVal);
	SetCoinSprite();
}

void AGameUI::CheckCoin()
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

void AGameUI::SetCoinSprite()
{
	unsigned __int8 coin = GameData::GetInstance().GetCoin();

	int base10 = coin / 10;
	int base1 = coin % 10;

	SRCreditCount[0]->SetSprite(SPRITE_TIME_COUNT, base10);
	SRCreditCount[1]->SetSprite(SPRITE_TIME_COUNT, base1);
}