#pragma once
#include <EngineCore/Actor.h>

class TitleLogo : public AActor
{
public:
	TitleLogo();
	~TitleLogo();

	TitleLogo(const TitleLogo& _Other) = delete;
	TitleLogo(TitleLogo&& _Other) noexcept = delete;
	TitleLogo& operator=(const TitleLogo& _Other) = delete;
	TitleLogo& operator=(TitleLogo&& _Other) noexcept = delete;

protected:
	void Tick(float _DeltaTime) override;

private:
	class USpriteRenderer* SpriteRenderer= nullptr;
};