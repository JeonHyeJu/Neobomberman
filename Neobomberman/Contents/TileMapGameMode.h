#pragma once
#include <EngineCore/GameMode.h>

#include "TileMap.h"

class ATileMapGameMode : public AGameMode
{
public:
	ATileMapGameMode();
	~ATileMapGameMode();

	ATileMapGameMode(const ATileMapGameMode& _Other) = delete;
	ATileMapGameMode(ATileMapGameMode&& _Other) noexcept = delete;
	ATileMapGameMode& operator=(const ATileMapGameMode& _Other) = delete;
	ATileMapGameMode& operator=(ATileMapGameMode&& _Other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

private:
	ATileMap* WallTileMap = nullptr;
	ATileMap* BoxTileMap = nullptr;
	ATileMap* GroundTileMap = nullptr;

	std::vector<ATileMap*> TilePtrs;
	int CurTilePtrIdx = 0;
};

