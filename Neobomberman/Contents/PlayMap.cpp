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
	//USpriteRenderer* SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	//SpriteRenderer->SetOrder(ERenderOrder::BACKGROUND);
	//SpriteRenderer->SetSprite("Bg_1-1.png");

	//FVector2D MapScale = SpriteRenderer->SetSpriteScale(1.0f);
	//SpriteRenderer->SetComponentLocation(MapScale.Half());

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
	InitMap();
	InitBombManager();
}

void APlayMap::Tick(float _deltaTime)
{
	bool hasExploded = BombManager->HasExplodedBomb();
	if (hasExploded)
	{
		HandleExplode();
	}
}

void APlayMap::InitMap()
{
	FIntPoint titleIdxs = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { titleIdxs.X * tileSize.X, titleIdxs.Y * tileSize.Y };
	FVector2D subSize = winSize - mapSize;
	FIntPoint moveLoc{ static_cast<int>(subSize.X / 2), GlobalVar::STAGE_H_MARGIN };

	{
		MapGround = GetWorld()->SpawnActor<ATileMap>();
		MapGround->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Ground);

		for (int y = 0; y < titleIdxs.Y; y++)
		{
			for (int x = 0; x < titleIdxs.X; x++)
			{
				MapGround->SetTile({ x, y }, static_cast<int>(TileType::Ground), true);
			}
		}

		MapGround->SetActorLocation(moveLoc);
	}

	{
		MapWall = GetWorld()->SpawnActor<ATileMap>();
		MapWall->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Wall);
		MapWall->SetActorLocation(moveLoc);
	}

	{
		MapBox = GetWorld()->SpawnActor<ATileMap>();
		MapBox->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Box);
		MapBox->SetActorLocation(moveLoc);
	}

	GlobalPath path;
	std::string tileDatPath = path.GetTileDataPath();

	Deserialize(MapWall, tileDatPath, GlobalPath::MAP_WALL_DAT);
	Deserialize(MapBox, tileDatPath, GlobalPath::MAP_BOX_DAT);

	// Temp
	MapBox->SetTilesAnim("CrumblingBox", "CrumblingBox");
}

void APlayMap::InitBombManager()
{
	// TODO: Intergrate with upper code
	FIntPoint titleIdxs = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { titleIdxs.X * tileSize.X, titleIdxs.Y * tileSize.Y };
	FVector2D subSize = winSize - mapSize;
	FIntPoint moveLoc{ static_cast<int>(subSize.X / 2), GlobalVar::STAGE_H_MARGIN };

	BombManager = GetWorld()->SpawnActor<ABombManager>();
	BombManager->Init(GlobalVar::BATTLE_GROUND_COUNT, GlobalVar::BOMB_SIZE);
	BombManager->SetActorLocation(moveLoc);
}

void APlayMap::HandleExplode()
{
	const std::vector<FIntPoint>& vec = BombManager->GetExplodedTileIdxs();
	for (size_t i = 0; i < vec.size(); ++i)
	{
		MapBox->LaunchTileAnim(vec[i], "CrumblingBox");	// temp
	}
	BombManager->ClearExplodeTileIdxs();
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
