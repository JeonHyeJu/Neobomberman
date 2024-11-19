#include "PreCompile.h"
#include "ContentsEnum.h"
#include "Fade.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineAPICore.h>
#include <functional>

AFade* AFade::MainFade = nullptr;

AFade::AFade()
{
	SRScreen = CreateDefaultSubObject<USpriteRenderer>();
	SRScreen->SetOrder(ERenderOrder::FADE);
	SRScreen->SetSprite("Fade.bmp");

	FVector2D MapScale = SRScreen->SetSpriteScale(1.0f);
	SRScreen->SetComponentLocation(MapScale.Half());
	SRScreen->SetCameraEffect(false);
}

AFade::~AFade()
{
}

void AFade::FadeIn()
{
	IsFadeEnd = false;
	FadeValue = 1.0f;
	FadeDir = -1.0f;
	TimeEventer.PushEvent(2.f, std::bind(&AFade::FadeChange, this), true, false);
}

void AFade::FadeOut()
{
	IsFadeEnd = false;
	FadeValue = 0.0f;
	FadeDir = 1.0f;
	TimeEventer.PushEvent(2.f, std::bind(&AFade::FadeChange, this), true, false);
}

void AFade::FadeChange()
{
	if (IsFadeEnd) return;

	float DeltaTime = UEngineAPICore::GetCore()->GetDeltaTime();
	FadeValue += DeltaTime * FadeSpeed * FadeDir;

	SRScreen->SetAlphafloat(FadeValue);

	if (std::abs(FadeDir - 1.0f) < 1e-4)	// FadeOut
	{
		if (FadeValue >= MaxFadeVal)
		{
			IsFadeEnd = true;

			for (std::function<void()>& fn : EndEvents)
			{
				fn();
			}
		}
	}
	else
	{
		if (FadeValue <= MinFadeVal)
		{
			IsFadeEnd = true;
		}
		// TODO..
	}
}

void AFade::BindEndEvent(std::function<void()> _fn)
{
	EndEvents.push_back(_fn);
}