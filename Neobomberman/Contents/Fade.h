#pragma once
#include <EngineCore/Actor.h>
#include <list>

class AFade : public AActor
{
public:
	static AFade* MainFade;

public:
	AFade();
	~AFade();

	AFade(const AFade& _Other) = delete;
	AFade(AFade&& _Other) noexcept = delete;
	AFade& operator=(const AFade& _Other) = delete;
	AFade& operator=(AFade&& _Other) noexcept = delete;

	void FadeIn(float _startVal = -99);
	void FadeOut(float _startVal = -99);
	void BindEndEvent(std::function<void()> _fn);

	void SetFadeSpeed(float _val)
	{
		FadeSpeed = _val;
	}

	void SetFadeMinMax(float _min = 0.f, float _max = 1.f)
	{
		MinFadeVal = _min;
		MaxFadeVal = _max;
	}

protected:
	void LevelChangeStart() override
	{
		MainFade = this;
	}

private:
	void FadeChange();

	class USpriteRenderer* SRScreen = nullptr;
	float FadeValue = 0.0f;
	float FadeDir = 1.0f;

	std::function<void()> EndEvent = nullptr;
	bool IsFadeEnd = false;

	float MaxFadeVal = 1.f;
	float MinFadeVal = 0.f;
	float FadeSpeed = 1.f;
};