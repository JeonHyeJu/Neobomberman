#include "PreCompile.h"
#include "PlayMap.h"
#include "ContentsEnum.h"
#include "TileMap.h"
#include "BombManager.h"
#include "GlobalVar.h"

#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/ImageManager.h>

APlayMap::APlayMap()
{
	USpriteRenderer* OuterLineRenderer = CreateDefaultSubObject<USpriteRenderer>();
	OuterLineRenderer->SetOrder(ERenderOrder::COLMAP);
	OuterLineRenderer->SetSprite("Bg_1-Col.png");

	FVector2D MapScale = OuterLineRenderer->SetSpriteScale(1.0f);
	OuterLineRenderer->SetSpriteScale(1.0f);
	OuterLineRenderer->SetComponentLocation(MapScale.Half());
	OuterLineRenderer->SetActive(false);
}

APlayMap::~APlayMap()
{
}

void APlayMap::BeginPlay()
{
	Super::BeginPlay();

	FIntPoint tileIdxs = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { tileIdxs.X * tileSize.X, tileIdxs.Y * tileSize.Y };
	FVector2D subSize = winSize - mapSize;
	FIntPoint moveLoc{ static_cast<int>(subSize.X / 2), GlobalVar::STAGE_H_MARGIN };

	InitMap(tileIdxs, tileSize, moveLoc);
	InitBombManager(tileIdxs, tileSize, moveLoc);
}

void APlayMap::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	checkExplodedBombs();
}

void APlayMap::InitMap(const FIntPoint& _tileIdxs, const FVector2D& _tileSize, const FIntPoint& _moveLoc)
{
	MapGround = GetWorld()->SpawnActor<ATileMap>();
	MapGround->Init(GlobalPath::TILE_STAGE_1, _tileIdxs, _tileSize, TileType::Ground);
	for (int y = 0; y < _tileIdxs.Y; y++)
	{
		for (int x = 0; x < _tileIdxs.X; x++)
		{
			MapGround->SetTile({ x, y }, static_cast<int>(TileType::Ground), true);
		}
	}
	MapGround->SetActorLocation(_moveLoc);

	MapWall = GetWorld()->SpawnActor<ATileMap>();
	MapWall->Init(GlobalPath::TILE_STAGE_1, _tileIdxs, _tileSize, TileType::Wall);
	MapWall->SetActorLocation(_moveLoc);

	MapBox = GetWorld()->SpawnActor<ATileMap>();
	MapBox->Init(GlobalPath::TILE_STAGE_1, _tileIdxs, _tileSize, TileType::Box);
	MapBox->SetActorLocation(_moveLoc);

	GlobalPath path;
	std::string tileDatPath = path.GetTileDataPath();

	Deserialize(MapWall, tileDatPath, GlobalPath::MAP_WALL_DAT);
	Deserialize(MapBox, tileDatPath, GlobalPath::MAP_BOX_DAT);

	MapBox->SetTilesAnimAfterLoad(GlobalPath::ANIM_CRUMBLING_BOX, GlobalPath::ANIM_CRUMBLING_BOX);
}

void APlayMap::InitBombManager(const FIntPoint& _tileIdxs, const FVector2D& _tileSize, const FIntPoint& _moveLoc)
{
	BombManager = GetWorld()->SpawnActor<ABombManager>();
	BombManager->Init(_tileIdxs, _tileSize);
	BombManager->SetActorLocation(_moveLoc);
}

void APlayMap::checkExplodedBombs()
{
	bool hasExploded = BombManager->HasExplodedBomb();
	if (hasExploded)
	{
		const std::vector<FIntPoint>& vec = BombManager->GetExplodedTileIdxs();
		for (size_t i = 0; i < vec.size(); ++i)
		{
			FIntPoint boxIdx = vec[i];
			MapBox->LaunchTileAnimAfterLoad(boxIdx, GlobalPath::ANIM_CRUMBLING_BOX);	// temp
		}
		BombManager->ClearExplodeTileIdxs();
	}
}

bool APlayMap::Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName)
{
	if (_tileMap == nullptr) return false;

	UEngineDirectory dir;
	dir.MoveRelative(_savePath);

	UEngineSerializer serializer;
	UEngineFile file = dir.GetPathToString() + "\\" + _saveName.data();
	file.FileOpen("rb");
	file.Read(serializer);
	_tileMap->DeSerialize(serializer);
	file.Close();

	return true;
}
