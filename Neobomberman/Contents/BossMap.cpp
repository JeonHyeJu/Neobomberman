#include "PreCompile.h"
#include "BossMap.h"
#include "TileMap.h"
#include "Bomb.h"

#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/ImageManager.h>

ABossMap::ABossMap()
: ABaseMap()
{

}

ABossMap::~ABossMap()
{

}

void ABossMap::BeginPlay()
{
	ABaseMap::BeginPlay();
}

void ABossMap::Tick(float _deltaTime)
{
	ABaseMap::Tick(_deltaTime);
}

void ABossMap::InitMap()
{
	FIntPoint shape = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { shape.X * tileSize.X, shape.Y * tileSize.Y };
	FVector2D subSize = winSize - mapSize;
	FIntPoint moveLoc{ static_cast<int>(subSize.X / 2), GlobalVar::STAGE_H_MARGIN };

	MapGround = GetWorld()->SpawnActor<ATileMap>();
	MapGround->Init(GlobalPath::TILE_STAGE_1, shape, tileSize, TileType::Ground);
	for (int y = 0; y < shape.Y; y++)
	{
		for (int x = 0; x < shape.X; x++)
		{
			MapGround->SetTile({ x, y }, 5, true);
		}
	}
	MapGround->SetActorLocation(moveLoc);

	MapWall = GetWorld()->SpawnActor<ATileMap>();
	MapWall->Init(GlobalPath::TILE_STAGE_1, shape, tileSize, TileType::Wall);
	MapWall->SetActorLocation(moveLoc);

	MapBox = GetWorld()->SpawnActor<ATileMap>();
	MapBox->Init(GlobalPath::TILE_STAGE_1, shape, tileSize, TileType::Box);
	MapBox->SetActorLocation(moveLoc);

	GlobalPath path;
	std::string tileDatPath = path.GetTileDataPath();

	Deserialize(MapWall, tileDatPath, "BossWallData.dat");	// Temp
}
