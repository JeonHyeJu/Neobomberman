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

void ABossMap::InitSprite()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	{
		const char* SPRITE_BG = "PlayBackground";

		SRBackground = CreateDefaultSubObject<USpriteRenderer>();
		SRBackground->SetSprite(SPRITE_BG);
		SRBackground->SetComponentLocation(winSize.Half());
		SRBackground->SetComponentScale(winSize);
		SRBackground->SetOrder(ERenderOrder::BACKGROUND);
		SRBackground->CreateAnimation(ANIM_RUN_BG, SPRITE_BG, 0, 9, 1.f);
	}

	{
		const char* SPRITE_BG_BOTTOM = "BgBottom.png";

		FVector2D size{ 416.f, 32.f };
		SRBackgroundBottom = CreateDefaultSubObject<USpriteRenderer>();
		SRBackgroundBottom->SetSprite(SPRITE_BG_BOTTOM, 1);
		SRBackgroundBottom->SetComponentLocation({ winSize.hX(), winSize.Y - size.hY() });
		SRBackgroundBottom->SetComponentScale(size);
		SRBackgroundBottom->SetOrder(999);
	}

	// TODO
	{
		const char* SPRITE_LEFT_BEAR = "LeftBears.png";
		const char* SPRITE_RIGHT_BEAR = "RightBears.png";

		FVector2D size = { 96, 196 };
		SRLeftBear = CreateDefaultSubObject<USpriteRenderer>();
		SRLeftBear->SetSprite(SPRITE_LEFT_BEAR);
		SRLeftBear->SetComponentLocation(size.Half() + FVector2D{ -2.f, 150.f });
		SRLeftBear->SetComponentScale(size);
		SRLeftBear->SetOrder(ERenderOrder::BACKGROUND_OVER);
		SRLeftBear->CreateAnimation(ANIM_RUN_BG, SPRITE_LEFT_BEAR, 0, 7, 1.f);

		SRRightBear = CreateDefaultSubObject<USpriteRenderer>();
		SRRightBear->SetSprite(SPRITE_RIGHT_BEAR);
		SRRightBear->SetComponentLocation(size.Half() + FVector2D{ winSize.X - size.X + 2, 150.f });
		SRRightBear->SetComponentScale(size);
		SRRightBear->SetOrder(ERenderOrder::BACKGROUND_OVER);
		SRRightBear->CreateAnimation(ANIM_RUN_BG, SPRITE_RIGHT_BEAR, 0, 7, 1.f);
	}

	SRBackground->ChangeAnimation(ANIM_RUN_BG);
	SRLeftBear->ChangeAnimation(ANIM_RUN_BG);
	SRRightBear->ChangeAnimation(ANIM_RUN_BG);
}

void ABossMap::InitTileMap()
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
			MapGround->SetTile({ x, y }, 1, true);
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

	Deserialize(MapWall, tileDatPath, std::string("Boss") + GlobalPath::MAP_WALL_DAT);
}
