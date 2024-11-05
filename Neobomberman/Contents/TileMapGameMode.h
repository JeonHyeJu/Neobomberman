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
	void InitMouseRenderer();
	void MoveMouseRenderer(ATileMap* _curMapPtr, const FVector2D& _mousePos);
	void UpdateMouseRenderer();
	bool Serialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);

	ATileMap* GroundTileMap = nullptr;
	ATileMap* WallTileMap = nullptr;
	ATileMap* BoxTileMap = nullptr;

	class USpriteRenderer* MouseSpriteRender = nullptr;

	const char* TILE_DAT_PATH = "Resources\\Tiles\\Data";
	const char* TILE_IMG_FOLDER_NAME = "TileStage_1";
	const char* TILE_GUIDE_IMG_FOLDER_NAME = "TileStage_1_Guide";

	std::vector<ATileMap*> TilePtrs;
	int CurTilePtrIdx = 0;
	FIntPoint MouseIdx;
};

