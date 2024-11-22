#include "PreCompile.h"
#include "BaseMap.h"
#include "TileMap.h"
#include "Bomb.h"
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>

ABaseMap::ABaseMap()
: AActor()
{

}

ABaseMap::~ABaseMap()
{

}

void ABaseMap::BeginPlay()
{
	Super::BeginPlay();

	ABomb::BombList.clear();
	SplashTileIdxs.clear();
}

void ABaseMap::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	CheckLaunchedBomb();
	RemoveExplodedBomb();
	CheckExplodedBox();
	if (SplashTileIdxs.size() > 0 && ExplodeEvent != nullptr)
	{
		ExplodeEvent();
	}
	ClearSplashArray();
}

bool ABaseMap::Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName)
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

bool ABaseMap::CanMove(const FVector2D& _loc)
{
	bool canMoveWall = true;
	bool canMoveBox = true;

	if (MapWall)
	{
		canMoveWall = MapWall->GetIsMovable(_loc);
	}

	if (MapBox)
	{
		canMoveBox = MapBox->GetIsMovable(_loc);
	}
	
	bool canMove = canMoveWall && canMoveBox;
	return canMove;
}

bool ABaseMap::CanMove(const FIntPoint& _idx)
{
	bool hasWall = MapWall->IsBlocked(_idx);
	bool hasBox = MapBox->IsBlocked(_idx);

	bool isBlocked = hasWall || hasBox;
	return !isBlocked;
}

bool ABaseMap::GetIsPortalOpened() const
{
	if (MapGround)
	{
		return MapGround->GetIsPortalOpened();
	}
	return false;
}

void ABaseMap::OpenPortal()
{
	// Don't have any portal.
	if (PortalIdx != FIntPoint::NEGATIVE_ONE)
	{
		if (MapGround)
		{
			MapGround->OpenPortal();
		}
	}
}

FVector2D ABaseMap::GetPortalLoc() const
{
	if (MapGround == nullptr || PortalIdx == FIntPoint::NEGATIVE_ONE)
	{
		return FVector2D::ZERO;
	}

	FVector2D realLoc = MapGround->GetActorLocation() + MapGround->IndexToLocation(PortalIdx);	// include margin
	return realLoc;
}

FIntPoint ABaseMap::LocationToMatrixIdx(const FVector2D& _loc)
{
	if (!MapGround) return FIntPoint::NEGATIVE_ONE;
	return MapGround->LocationToMatrixIdx(_loc);
}

FVector2D ABaseMap::MatrixIdxToLocation(const FIntPoint& _idx)
{
	if (!MapGround) return FIntPoint::NEGATIVE_ONE;
	return MapGround->LocationToMatrixIdx(_idx);
}

void ABaseMap::CheckLaunchedBomb()
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

void ABaseMap::RemoveExplodedBomb()
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

void ABaseMap::CheckExplodedBox()
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

void ABaseMap::ClearSplashArray()
{
	SplashTileIdxs.clear();
}

void ABaseMap::AppendSplash(const std::vector<FIntPoint>& _appendIdxs)
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

bool ABaseMap::IsMove(const FIntPoint& _Point)
{
	bool hasWall = false;
	bool hasBox = false;

	// Temp
	bool isPortal = _Point == FIntPoint({ 6, 10 });

	if (MapWall)
	{
		hasWall = MapWall->IsBlocked(_Point);
	}
	if (MapBox)
	{
		hasBox = MapBox->IsBlocked(_Point);
	}

	/*bool hasWall = MapWall->GetIsMovable(_Point);
	bool hasBox = MapBox->GetIsMovable(_Point);*/

	// TODO!!: hasBomb

	bool isBlocked = hasWall || hasBox || isPortal;
	return !isBlocked;
}