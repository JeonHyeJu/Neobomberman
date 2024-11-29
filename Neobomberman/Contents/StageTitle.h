#pragma once
#include <EngineCore/Actor.h>

class AStageTitle : public AActor
{
public:
	AStageTitle();
	~AStageTitle();

	AStageTitle(const AStageTitle& _other) = delete;
	AStageTitle(AStageTitle&& _other) noexcept = delete;
	AStageTitle& operator=(const AStageTitle& _other) = delete;
	AStageTitle& operator=(AStageTitle&& _other) noexcept = delete;

	// Only asume stage 1-*
	inline void SetSubStage(int _subStage)
	{
		SubStage = _subStage;
	}

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	class USpriteRenderer* SRTitle = nullptr;
	int SubStage = 1;

	const char* SPRITE_STAGE_TITLE = "StageTitle.png";
	const char* ANIM_NAME = "BLINK";
	const int SHOWING_SECS = 3;
};
