#include "PreCompile.h"
#include "PlayMap.h"
#include "ContentsEnum.h"
#include "TileMap.h"
#include "BombManager.h"
#include "Bomb.h"
#include "GlobalVar.h"

#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/ImageManager.h>

void _ReorginizeExplosion(std::vector<EBombTailType>& _bombTrails)
{
	for (size_t i = 0, size = _bombTrails.size(); i < size - 1; ++i)
	{
		if (i + 1 >= size) break;

		if (_bombTrails[i] == EBombTailType::END && _bombTrails[i + 1] == EBombTailType::END)
		{
			_bombTrails[i] = EBombTailType::CONNECT;
		}
		else if (_bombTrails[i] == EBombTailType::END && _bombTrails[i + 1] == EBombTailType::CONNECT)
		{
			_bombTrails[i] = EBombTailType::CONNECT;
		}
	}
}

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
	//BombManager->Init(_tileIdxs, _tileSize);
	BombManager->SetActorLocation(_moveLoc);
}

void APlayMap::_AddExplosion(const FIntPoint& _orgIdx, const FIntPoint& direc, int _idx, std::vector<EBombTailType>& _vec, bool* isEnd)
{
	FIntPoint nextIdx = _orgIdx + direc * _idx;

	bool hasWall = MapWall->IsBlocked(nextIdx);
	bool hasBox = MapBox->IsBlocked(nextIdx);

	if (hasWall == false && *isEnd == false)
	{
		if (hasBox == true)
		{
			*isEnd = true;
			_vec.push_back(EBombTailType::CONNECT);
		}
		else
		{
			_vec.push_back(EBombTailType::END);
		}
	}
	else
	{
		*isEnd = true;
	}
}

void APlayMap::SetBomb(const FVector2D& _loc, EBombType _bombType, int _power)
{
	FVector2D size = GlobalVar::BOMB_SIZE;		// temp
	FIntPoint orgIdx = BombManager->LocationToMatrixIdx(_loc);

	bool isUpEnd = false;
	bool isDownEnd = false;
	bool isLeftEnd = false;
	bool isRightEnd = false;

	SBombTails bombExplosion;
	for (int i = 1; i <= _power; i++)
	{
		_AddExplosion(orgIdx, FIntPoint::UP, i, bombExplosion.Up, &isUpEnd);
		_AddExplosion(orgIdx, FIntPoint::DOWN, i, bombExplosion.Down, &isDownEnd);
		_AddExplosion(orgIdx, FIntPoint::LEFT, i, bombExplosion.Left, &isLeftEnd);
		_AddExplosion(orgIdx, FIntPoint::RIGHT, i, bombExplosion.Right, &isRightEnd);
	}

	_ReorginizeExplosion(bombExplosion.Up);
	_ReorginizeExplosion(bombExplosion.Down);
	_ReorginizeExplosion(bombExplosion.Left);
	_ReorginizeExplosion(bombExplosion.Right);

	BombManager->SetBomb(_loc, bombExplosion, _bombType, _power);
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
			MapBox->LaunchTileAnimAfterLoad(boxIdx, GlobalPath::ANIM_CRUMBLING_BOX);
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