#pragma once
#include <EngineCore/Actor.h>

class AEnding : public AActor
{
public:
	AEnding();
	~AEnding();

	AEnding(const AEnding& _other) = delete;
	AEnding(AEnding&& _other) noexcept = delete;
	AEnding& operator=(const AEnding& _other) = delete;
	AEnding& operator=(AEnding&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	void OnEndFadeOut();

	const char* ENDING_SPRITE_PATH = "Ending.png";
	const char* ANIM_IDLE_NAME = "Closing_Idle";
	const char* ANIM_RUN_NAME = "Closing_Run";

	class USpriteRenderer* SpriteRenderer = nullptr;

	/* Sounds */
	const char* SFXBg = "Intermission.mp3";
};
