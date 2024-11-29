#include "PreCompile.h"
#include "BaseMap.h"
#include "TileMap.h"
#include "Bomb.h"
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>

ABaseMap::ABaseMap()
: AActor()
{
	Items.reserve(GlobalVar::MAX_MAP_ITEM_CNT);
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

bool ABaseMap::CanMove(const FVector2D& _loc, bool _isPlayer /*= false*/)
{
	FIntPoint _idx = LocationToMatrixIdx(_loc);

	std::list<ABomb*>::iterator it = ABomb::BombList.begin();
	std::list<ABomb*>::iterator itEnd = ABomb::BombList.end();
	for (; it != itEnd; ++it)
	{
		ABomb* pBomb = *it;
		if (_idx == pBomb->GetMatrixIdx())
		{
			if (_isPlayer)
			{
				if (!pBomb->GetIsMovableForPlayer())
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}

	if (MapWall)
	{
		if (!MapWall->GetIsMovable(_loc))
		{
			return false;
		}
	}

	if (MapBox)
	{
		if (!MapBox->GetIsMovable(_loc))
		{
			return false;
		}
	}

	return true;
}

bool ABaseMap::IsMove(const FIntPoint& _idx)
{
	std::list<ABomb*>::iterator it = ABomb::BombList.begin();
	std::list<ABomb*>::iterator itEnd = ABomb::BombList.end();
	for (; it != itEnd; ++it)
	{
		ABomb* pBomb = *it;

		if (_idx == pBomb->GetMatrixIdx())
		{
			return false;
		}
	}

	if (_idx == PortalIdx)
	{
		return false;
	}

	if (MapWall)
	{
		//if (!MapWall->GetIsMovable(_idx))
		if (MapWall->HasTileSprite(_idx))
		{
			return false;
		}
	}
	if (MapBox)
	{
		//if (!MapBox->GetIsMovable(_idx))
		if (MapBox->HasTileSprite(_idx))
		{
			return false;
		}
	}

	return true;
}

void ABaseMap::OpenPortal()
{
	// It dosen't have any portal.
	if (PortalIdx != FIntPoint::NEGATIVE_ONE)
	{
		if (MapGround)
		{
			MapGround->OpenPortal();
		}
	}
}

bool ABaseMap::GetIsPortalOpened() const
{
	if (MapGround)
	{
		return MapGround->GetIsPortalOpened();
	}
	return false;
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

FIntPoint ABaseMap::LocationToIndex(const FVector2D& _loc)
{
	if (!MapGround) return FIntPoint::NEGATIVE_ONE;
	return MapGround->LocationToIndex(_loc);
}

FVector2D ABaseMap::IndexToLocation(const FIntPoint& _idx)
{
	if (!MapGround) return FIntPoint::NEGATIVE_ONE;
	return MapGround->IndexToLocation(_idx);
}

FIntPoint ABaseMap::LocationToMatrixIdx(const FVector2D& _loc)
{
	if (!MapGround) return FIntPoint::NEGATIVE_ONE;
	return MapGround->LocationToMatrixIdx(_loc);
}

FVector2D ABaseMap::MatrixIdxToLocation(const FIntPoint& _idx)
{
	if (!MapGround) return FIntPoint::NEGATIVE_ONE;
	return MapGround->MatrixIdxToLocation(_idx);
}
FVector2D ABaseMap::GetOrganizedLoc(const FVector2D& _loc)
{
	FVector2D ret;
	if (!MapGround) return ret;

	FIntPoint idx = MapGround->LocationToMatrixIdx(_loc);
	ret = MapGround->MatrixIdxToLocation(idx);
	return ret;
}

/* Bomb */
bool ABaseMap::IsInSplash(const FIntPoint& _pt)
{
	return IsInSplashWithVector(_pt, SplashTileIdxs);
}

bool ABaseMap::IsInSplashWithVector(const FIntPoint& _pt, const std::vector<FIntPoint>& _vec)
{
	for (size_t i = 0, size = _vec.size(); i < size; ++i)
	{
		if (_pt == _vec[i])
		{
			return true;
		}
	}
	return false;
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

/* Item */
bool ABaseMap::HasItem(const FIntPoint& _idx)
{
	if (MapBox == nullptr) return false;
	return MapBox->HasItem(_idx);
}

bool ABaseMap::HasShowingItem(const FIntPoint& _idx)
{
	if (MapBox == nullptr) return false;
	bool hasItem = HasItem(_idx);
	bool isShowingItem = MapBox->IsShowingItem(_idx);
	return hasItem && isShowingItem;
}

EItem ABaseMap::PopItem(const FIntPoint& _idx)
{
	if (MapBox == nullptr) return EItem::NONE;
	return MapBox->PopItem(_idx);
}

/* Cheats */
void ABaseMap::CheatDestoyAllBoxes()
{
	if (MapBox)
	{
		MapBox->CheatDestoyAllBoxes();
	}
}