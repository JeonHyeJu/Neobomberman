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

void AFade::FadeIn(float _startVal)
{
	IsFadeEnd = false;
	FadeValue = (_startVal < -1 ? 1.0f : _startVal);
	FadeDir = -1.0f;
	TimeEventer.PushEvent(2.f, std::bind(&AFade::FadeChange, this), true, false);
}

void AFade::FadeOut(float _startVal)
{
	IsFadeEnd = false;
	FadeValue = (_startVal < -1 ? 0.0f : _startVal);
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

			if (EndEvent)
			{
				EndEvent();
				EndEvent = nullptr;
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

// TODO
void AFade::BindEndEvent(std::function<void()> _fn)
{
	EndEvent = _fn;
}