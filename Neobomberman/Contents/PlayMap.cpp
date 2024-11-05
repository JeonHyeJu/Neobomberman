#include "PreCompile.h"
#include "PlayMap.h"
#include "ContentsEnum.h"
#include "TileMap.h"
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
}

void APlayMap::InitMap()
{
	FIntPoint titleIdxs = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	int marginH = 32;		// temp
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { titleIdxs.X * tileSize.X, titleIdxs.Y * tileSize.Y };
	FVector2D subSize = winSize - mapSize;
	FIntPoint moveLoc{ static_cast<int>(subSize.X / 2), marginH };

	{
		MapGround = GetWorld()->SpawnActor<ATileMap>();
		MapGround->Init(TILE_IMG_FOLDER_NAME, titleIdxs, tileSize, TileType::Ground);

		for (int y = 0; y < titleIdxs.Y; y++)
		{
			for (int x = 0; x < titleIdxs.X; x++)
			{
				MapGround->SetSpriteAndIndex({ x, y }, static_cast<int>(TileType::Ground));
			}
		}

		MapGround->SetActorLocation(moveLoc);
	}

	{
		MapWall = GetWorld()->SpawnActor<ATileMap>();
		MapWall->Init(TILE_IMG_FOLDER_NAME, titleIdxs, tileSize, TileType::Wall);
		MapWall->SetActorLocation(moveLoc);
	}

	{
		MapBox = GetWorld()->SpawnActor<ATileMap>();
		MapBox->Init(TILE_IMG_FOLDER_NAME, titleIdxs, tileSize, TileType::Box);
		MapBox->SetActorLocation(moveLoc);
	}

	Deserialize(MapWall, TILE_DAT_PATH, "WallData.dat");
	Deserialize(MapBox, TILE_DAT_PATH, "BoxData.dat");
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