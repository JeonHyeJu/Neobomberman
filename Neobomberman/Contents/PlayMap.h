#pragma once
#include <EngineCore/Actor.h>

class ATileMap;
class APlayMap : public AActor
{
public:
	APlayMap();
	~APlayMap();

	APlayMap(const APlayMap& _Other) = delete;
	APlayMap(APlayMap&& _Other) noexcept = delete;
	APlayMap& operator=(const APlayMap& _Other) = delete;
	APlayMap& operator=(APlayMap&& _Other) noexcept = delete;

protected:
	 void BeginPlay() override;

private:
	void InitMap();
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);

	class ATileMap* MapGround = nullptr;
	class ATileMap* MapWall = nullptr;
	class ATileMap* MapBox = nullptr;

	const char* TILE_DAT_PATH = "Resources\\Tiles\\Data";
	const char* TILE_IMG_FOLDER_NAME = "TileStage_1";
};
