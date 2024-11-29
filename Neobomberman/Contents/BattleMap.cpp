#include "PreCompile.h"
#include "GlobalVar.h"
#include "BattleMap.h"
#include "TileMap.h"

#include <EngineCore/SpriteRenderer.h>

ABattleMap::ABattleMap()
: ABaseMap()
{
	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	{
		SRBackground = CreateDefaultSubObject<USpriteRenderer>();
		SRBackground->SetSprite(SPRITE_BATTLE_BG);
		SRBackground->SetComponentLocation(winSize.Half());
		SRBackground->SetComponentScale(winSize);
		SRBackground->SetOrder(ERenderOrder::BACKGROUND);
	}
	{
		FVector2D size{ 160, 96 };
		SRBgBuilding = CreateDefaultSubObject<USpriteRenderer>();
		SRBgBuilding->SetSprite(SPRITE_BATTLE_BUILDING);
		SRBgBuilding->SetComponentLocation(FVector2D{ winSize.hX(), size.hY()+32.f});
		SRBgBuilding->SetComponentScale(size);
		SRBgBuilding->SetOrder(ERenderOrder::CRUMBLING_BOX);
	}
}

ABattleMap::~ABattleMap()
{
}

void ABattleMap::BeginPlay()
{
	ABaseMap::BeginPlay();
}

void ABattleMap::Tick(float _deltaTime)
{
	ABaseMap::Tick(_deltaTime);
}

void ABattleMap::InitMap()
{
	FIntPoint shape = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { shape.X * tileSize.X, shape.Y * tileSize.Y };
	FVector2D subSize = winSize - mapSize;
	FIntPoint moveLoc{ static_cast<int>(subSize.X / 2), GlobalVar::STAGE_H_MARGIN };

	MapGround = GetWorld()->SpawnActor<ATileMap>();
	MapGround->Init(GlobalPath::TILE_STAGE_1, shape, tileSize, TileType::Ground);
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

	const std::string BATTLE = "Battle";	// Temp
	Deserialize(MapGround, tileDatPath, BATTLE + GlobalPath::MAP_GROUND_DAT);
	Deserialize(MapWall, tileDatPath, BATTLE + GlobalPath::MAP_WALL_DAT);
	Deserialize(MapBox, tileDatPath, BATTLE + GlobalPath::MAP_BOX_DAT);
	Deserialize(MapCover, tileDatPath, BATTLE + GlobalPath::MAP_COVER_DAT);

	MapBox->SetTilesAnimAfterLoad(GlobalPath::ANIM_CRUMBLING_BOX, "CrumblingWoodBox.png", 0, 6);
}
