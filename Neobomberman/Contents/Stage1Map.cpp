#include "PreCompile.h"
#include "Stage1Map.h"
#include "TileMap.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"

#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/ImageManager.h>

AStage1Map::AStage1Map()
: ABaseMap()
{
}

AStage1Map::~AStage1Map()
{
}

void AStage1Map::BeginPlay()
{
	ABaseMap::BeginPlay();
}

void AStage1Map::Tick(float _deltaTime)
{
	ABaseMap::Tick(_deltaTime);
}

void AStage1Map::InitSprite()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	{
		SRBackground = CreateDefaultSubObject<USpriteRenderer>();
		SRBackground->SetSprite("PlayBackground");
		SRBackground->SetComponentLocation(winSize.Half());
		SRBackground->SetComponentScale(winSize);
		SRBackground->SetOrder(ERenderOrder::BACKGROUND);
		SRBackground->CreateAnimation("Run", "PlayBackground", 0, 9, 1.f);
	}

	{
		FVector2D size{ 416.f, 32.f };
		SRBackgroundBottom = CreateDefaultSubObject<USpriteRenderer>();
		SRBackgroundBottom->SetSprite("BgBottom.png");
		SRBackgroundBottom->SetComponentLocation({ winSize.hX(), winSize.Y - size.hY() });
		SRBackgroundBottom->SetComponentScale(size);
		SRBackgroundBottom->SetOrder(999);
	}

	// TODO
	{
		FVector2D size = { 96, 196 };
		SRLeftBear = CreateDefaultSubObject<USpriteRenderer>();
		SRLeftBear->SetSprite("LeftBears.png");
		SRLeftBear->SetComponentLocation(size.Half() + FVector2D{ -2.f, 150.f });
		SRLeftBear->SetComponentScale(size);
		SRLeftBear->SetOrder(ERenderOrder::BACKGROUND_OVER);
		SRLeftBear->CreateAnimation("Run", "LeftBears.png", 0, 7, 1.f);

		SRRightBear = CreateDefaultSubObject<USpriteRenderer>();
		SRRightBear->SetSprite("RightBears.png");
		SRRightBear->SetComponentLocation(size.Half() + FVector2D{ winSize.X - size.X + 2, 150.f });
		SRRightBear->SetComponentScale(size);
		SRRightBear->SetOrder(ERenderOrder::BACKGROUND_OVER);
		SRRightBear->CreateAnimation("Run", "RightBears.png", 0, 7, 1.f);
	}

	SRBackground->ChangeAnimation("Run");
	SRLeftBear->ChangeAnimation("Run");
	SRRightBear->ChangeAnimation("Run");
}

void AStage1Map::InitTileMap()
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

	MapCover = GetWorld()->SpawnActor<ATileMap>();
	MapCover->Init(GlobalPath::TILE_STAGE_1, shape, tileSize, TileType::Cover);
	MapCover->SetActorLocation(moveLoc);
	MapCover->SetCustomOrder(ERenderOrder::MAP_COVER);

	GlobalPath path;
	std::string tileDatPath = path.GetTileDataPath();

	Deserialize(MapWall, tileDatPath, GlobalPath::MAP_WALL_DAT);
	Deserialize(MapBox, tileDatPath, GlobalPath::MAP_BOX_DAT);
	Deserialize(MapCover, tileDatPath, GlobalPath::MAP_COVER_DAT);

	MapBox->SetTilesAnimAfterLoad(GlobalPath::ANIM_CRUMBLING_BOX, GlobalPath::ANIM_CRUMBLING_BOX, 0, 10);
	MapGround->SetPortal(PortalIdx, { 0, 0 }, GlobalVar::BOMB_SIZE, "ClosedPortal.png");	// Temp
	MapBox->SetItemsAfterLoad(Items);
}

