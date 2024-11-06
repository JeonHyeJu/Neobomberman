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
	void InitMap();
	void InitMouseRenderer();
	void MoveMouseRenderer(ATileMap* _curMapPtr, const FVector2D& _mousePos);
	void UpdateMouseRenderer();
	void NextTile();
	void AddTile(ATileMap* _curMapPtr, const FVector2D& _mousePos);
	void RemoveTile(ATileMap* _curMapPtr, const FVector2D& _mousePos);
	void SaveTileMap();
	void LoadTileMap();

	bool Serialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);

	ATileMap* GroundTileMap = nullptr;
	ATileMap* WallTileMap = nullptr;
	ATileMap* BoxTileMap = nullptr;

	class USpriteRenderer* MouseSpriteRender = nullptr;
	std::string TileDataPath = "";

	std::vector<ATileMap*> TilePtrs;
	int CurTilePtrIdx = 0;
	FIntPoint MouseIdx;
};

