#include "PreCompile.h"
#include "PlayMap.h"
#include "TileMap.h"

#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/ImageManager.h>

APlayMap::APlayMap()
: ABaseMap()
{
}

APlayMap::~APlayMap()
{
}

void APlayMap::BeginPlay()
{
	ABaseMap::BeginPlay();
}

void APlayMap::Tick(float _deltaTime)
{
	ABaseMap::Tick(_deltaTime);
}

void APlayMap::InitMap()
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
			MapGround->SetTile({ x, y }, static_cast<int>(TileType::Ground), true);
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

	Deserialize(MapWall, tileDatPath, GlobalPath::MAP_WALL_DAT);
	Deserialize(MapBox, tileDatPath, GlobalPath::MAP_BOX_DAT);

	MapBox->SetTilesAnimAfterLoad(GlobalPath::ANIM_CRUMBLING_BOX, GlobalPath::ANIM_CRUMBLING_BOX);

	MapGround->SetPortal(PortalIdx, { 0, 0 }, GlobalVar::BOMB_SIZE, "ClosedPortal.png");	// Temp
}

bool APlayMap::IsMove(const FIntPoint& _Point)
{
	// Temp
	bool isPortal = _Point == FIntPoint({ 6, 10 });
	bool hasWall = MapWall->IsBlocked(_Point);
	bool hasBox = MapBox->IsBlocked(_Point);

	/*bool hasWall = MapWall->GetIsMovable(_Point);
	bool hasBox = MapBox->GetIsMovable(_Point);*/

	// TODO!!: hasBomb

	bool isBlocked = hasWall || hasBox || isPortal;
	return !isBlocked;
}

