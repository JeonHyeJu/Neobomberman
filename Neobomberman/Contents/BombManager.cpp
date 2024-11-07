#include "PreCompile.h"
#include "BombManager.h"
#include "GlobalVar.h"
#include <EngineCore/Level.h>
#include "Bomb.h"
#include <EngineCore/SpriteRenderer.h>

ABombManager::ABombManager()
{
}

ABombManager::~ABombManager()
{
}

void ABombManager::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	if (BombMatrix.size() == 0) return;

	for (int y = 0; y < BombMatrix.size(); ++y)
	{
		for (int x = 0; x < BombMatrix[y].size(); ++x)
		{
			if (BombMatrix[y][x] == nullptr) continue;

			if (BombMatrix[y][x]->State == BombState::Exploding)
			{
				BombMatrix[y][x]->SetState(BombState::FinishExploding);
				std::vector<FIntPoint> explodeIdxs = GetExplodeIdxs(FIntPoint{x, y}, BombMatrix[y][x]->Power);
				AppendExplodeTiles(explodeIdxs);
			}
			else if (BombMatrix[y][x]->State == BombState::Over)
			{
				BombMatrix[y][x]->Destroy();
				BombMatrix[y][x] = nullptr;
			}
		}
	}
}

void ABombManager::Init(const FIntPoint& _Count, const FVector2D& _TileSize)
{
	TileCount = _Count;
	TileSize = _TileSize;

	BombMatrix.resize(_Count.Y);

	for (size_t y = 0; y < BombMatrix.size(); y++)
	{
		BombMatrix[y].resize(_Count.X);
	}
}

FVector2D ABombManager::IndexToLocation(const FIntPoint& _Index)
{
	return FVector2D(_Index.X * TileSize.X, _Index.Y * TileSize.Y);
}

FIntPoint ABombManager::LocationToIndex(const FVector2D& _Location)
{
	FVector2D Location = _Location / TileSize;

	return FIntPoint(Location.iX(), Location.iY());
}

std::vector<FIntPoint> ABombManager::GetExplodeIdxs(const FIntPoint& _idx, int _power)
{
	std::vector<FIntPoint> vec;
	vec.reserve(_power*4 + 1);
	vec.push_back(_idx);

	for (int i = 0; i < _power; ++i)
	{
		int addVal = i + 1;
		FIntPoint up{ _idx.X, _idx.Y + addVal };
		FIntPoint down{ _idx.X, _idx.Y - addVal };
		FIntPoint left{ _idx.X - addVal, _idx.Y };
		FIntPoint right{ _idx.X + addVal, _idx.Y };

		vec.push_back(up);
		vec.push_back(down);
		vec.push_back(left);
		vec.push_back(right);
	}

	return vec;
}

void ABombManager::SetBomb(const FIntPoint& _idx, EBombType _bombType, int _power)
{
	if (IsIndexOver(_idx) == true)
	{
		return;
	}

	FIntPoint matrixIdx = _idx - FIntPoint{ 3, 2 };	// Temp
	ABomb* pBomb = BombMatrix[matrixIdx.Y][matrixIdx.X];
	if (pBomb) return;

	pBomb = GetWorld()->SpawnActor<ABomb>();
	FVector2D loc = IndexToLocation(_idx);

	if (_bombType == EBombType::RED)
	{
		pBomb->InitRedBomb(loc, _power);
	}
	else // EBombType::PLAIN
	{
		pBomb->InitOrgBomb(loc, _power);
	}

	BombMatrix[matrixIdx.Y][matrixIdx.X] = pBomb;
}

void ABombManager::SetBomb(const FVector2D& _Location, EBombType _bombType, int _power)
{
	FIntPoint idx = LocationToIndex(_Location);
	SetBomb(idx, _bombType, _power);
}

ABomb* ABombManager::GetBombRef(const FIntPoint& _idx)
{
	if (IsIndexOver(_idx) == true)
	{
		return nullptr;
	}
	return BombMatrix[_idx.Y][_idx.X];
}

ABomb* ABombManager::GetBombRef(const FVector2D& _refLoc)
{
	FVector2D actorLoc = GetActorLocation();
	FIntPoint idx = LocationToIndex(_refLoc - actorLoc);
	return GetBombRef(idx);
}

bool ABombManager::IsIndexOver(const FIntPoint& _Index)
{
	if (0 > _Index.X)
	{
		return true;
	}

	if (0 > _Index.Y)
	{
		return true;
	}

	if (TileCount.X - 1 < _Index.X)
	{
		return true;
	}

	if (TileCount.Y - 1 < _Index.Y)
	{
		return true;
	}

	return false;
}

bool ABombManager::HasExplodedBomb()
{
	bool ret = (ExplodeTileIdxs.size() > 0);
	return ret;
}

void ABombManager::AppendExplodeTiles(const std::vector<FIntPoint>& _appendIdxs)
{
	for (size_t i = 0; i < _appendIdxs.size(); ++i)
	{
		if (_appendIdxs[i].X < 0 || _appendIdxs[i].Y < 0)
		{
			continue;
		}

		ExplodeTileIdxs.push_back(_appendIdxs[i]);
	}
}

void ABombManager::ClearExplodeTileIdxs()
{
	ExplodeTileIdxs.clear();
}