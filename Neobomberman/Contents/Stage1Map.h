#pragma once
#include "BaseMap.h"
#include "ContentsStruct.h"

class AStage1Map : public ABaseMap
{
public:
	AStage1Map();
	~AStage1Map();

	AStage1Map(const AStage1Map& _Other) = delete;
	AStage1Map(AStage1Map&& _Other) noexcept = delete;
	AStage1Map& operator=(const AStage1Map& _Other) = delete;
	AStage1Map& operator=(AStage1Map&& _Other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void InitSprite() override;
	void InitTileMap() override;

private:
	class USpriteRenderer* SRBackground = nullptr;
	class USpriteRenderer* SRBackgroundBottom = nullptr;
	class USpriteRenderer* SRLeftBear = nullptr;
	class USpriteRenderer* SRRightBear = nullptr;
};
