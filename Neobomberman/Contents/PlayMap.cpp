#include "PreCompile.h"
#include "PlayMap.h"
#include "TileMap.h"
#include "Bomb.h"

#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/ImageManager.h>

APlayMap::APlayMap()
{
}

APlayMap::~APlayMap()
{
}

void APlayMap::BeginPlay()
{
	Super::BeginPlay();

	ABomb::BombList.clear();
	SplashTileIdxs.clear();
}

void APlayMap::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	CheckLaunchedBomb();
	RemoveExplodedBomb();
	CheckExplodedBox();
	ClearSplashArray();

	static int cnt = 0;
	static bool once = true;
	if (cnt++ > 100)
	{
		cnt = 0;
		if (once)
		{
			once = false;

			if (MapGround)
			{
				return MapGround->SetPortalState(true);
			}
		}
	}
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

void APlayMap::CheckLaunchedBomb()
{
	std::list<ABomb*>::iterator it = ABomb::BombList.begin();
	std::list<ABomb*>::iterator itEnd = ABomb::BombList.end();

	for (; it != itEnd; ++it)
	{
		ABomb* pBomb = *it;
		if (pBomb == nullptr) continue;

		if (pBomb->GetState() == EBombState::Launched)
		{
			AppendSplash(pBomb->ExplodeIdxs);
		}
	}

	it = ABomb::BombList.begin();
	for (; it != itEnd; ++it)
	{
		ABomb* pBomb = *it;
		if (pBomb == nullptr) continue;

		if (pBomb->GetState() == EBombState::Running)
		{
			bool isInSplash = pBomb->IsInSplash(SplashTileIdxs);
			if (isInSplash)
			{
				pBomb->ExplodeBySplash();
			}
		}
	}
}

void APlayMap::RemoveExplodedBomb()
{
	std::list<ABomb*>::iterator it = ABomb::BombList.begin();
	std::list<ABomb*>::iterator itEnd = ABomb::BombList.end();

	for (; it != itEnd; ++it)
	{
		ABomb* pBomb = *it;
		if (pBomb != nullptr && pBomb->GetState() == EBombState::Over)
		{
			pBomb->Destroy();
			pBomb = nullptr;
			*it = nullptr;
		}
	}
	ABomb::BombList.remove(nullptr);
}

void APlayMap::CheckExplodedBox()
{
	bool hasExploded = SplashTileIdxs.size() > 0;
	if (hasExploded)
	{
		for (size_t i = 0; i < SplashTileIdxs.size(); ++i)
		{
			FIntPoint boxIdx = SplashTileIdxs[i];
			MapBox->LaunchTileAnimAfterLoad(boxIdx, GlobalPath::ANIM_CRUMBLING_BOX);
		}
	}
}

void APlayMap::ClearSplashArray()
{
	SplashTileIdxs.clear();
}

void APlayMap::AppendSplash(const std::vector<FIntPoint>& _appendIdxs)
{
	for (size_t i = 0; i < _appendIdxs.size(); ++i)
	{
		if (_appendIdxs[i].X < 0 || _appendIdxs[i].Y < 0)
		{
			continue;
		}

		for (size_t j = 0, size = SplashTileIdxs.size(); j < size; ++j)
		{
			if (_appendIdxs[i] == SplashTileIdxs[j])
			{
				break;
			}
		}

		SplashTileIdxs.push_back(_appendIdxs[i]);
	}
}

FVector2D APlayMap::GetPortalLoc() const
{
	if (MapGround == nullptr)
	{
		return FVector2D::ZERO;
	}

	FVector2D realLoc = MapGround->GetActorLocation() + MapGround->IndexToLocation(PortalIdx);	// include margin
	return realLoc;
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

bool APlayMap::GetIsPortalOpened() const
{
	if (MapGround)
	{
		return MapGround->GetIsOpenedPortal();
	}
	return false;
}