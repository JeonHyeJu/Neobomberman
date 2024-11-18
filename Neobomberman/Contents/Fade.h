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

	void FadeIn();
	void FadeOut();
	void BindEndEvent(std::function<void()> _fn);

protected:
	void LevelChangeStart()
	{
		MainFade = this;
	}

private:
	void FadeChange();

	class USpriteRenderer* SRScreen = nullptr;
	float FadeValue = 0.0f;
	float FadeDir = 1.0f;

	std::list<std::function<void()>> EndEvents;
	bool IsFadeEnd = false;
};