#pragma once
#include <EngineCore/Actor.h>

class ATitle : public AActor
{
public:
	ATitle();
	~ATitle();

	ATitle(const ATitle& _Other) = delete;
	ATitle(ATitle&& _Other) noexcept = delete;
	ATitle& operator=(const ATitle& _Other) = delete;
	ATitle& operator=(ATitle&& _Other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

private:
	void OnEndAnimation();

	const char* ANIM_IDLE_NAME = "Opening_Idle";
	const char* ANIM_RUN_NAME = "Opening_Run";

	class USpriteRenderer* SpriteRenderer = nullptr;
};