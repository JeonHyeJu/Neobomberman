#pragma once
#include <EngineCore/Actor.h>

class AHurryUp : public AActor
{
public:
	AHurryUp();
	~AHurryUp();

	AHurryUp(const AHurryUp& _other) = delete;
	AHurryUp(AHurryUp&& _other) noexcept = delete;
	AHurryUp& operator=(const AHurryUp& _other) = delete;
	AHurryUp& operator=(AHurryUp&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	const char* SPRITE_PATH = "HurryUp.png";
	const char* ANIM_NAME = "Run";
	class USpriteRenderer* SRHurryUp = nullptr;

	float Speed = 2500.f;
	FVector2D Size;
	bool IsStop = false;
	bool IsDestoiable = false;
};
