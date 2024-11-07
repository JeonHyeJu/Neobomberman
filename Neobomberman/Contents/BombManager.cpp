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
				std::vector<FIntPoint> explodeIdxs = GetExplodedIdxs(FIntPoint{x, y}, BombMatrix[y][x]->Power);
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

void ABombManager::SetBomb(const FVector2D& _realLoc, EBombType _bombType, int _power)
{
	FIntPoint subtractedIdx = LocationToMatrixIdx(_realLoc);
	if (IsIndexOver(subtractedIdx) == true)
	{
		return;
	}

	ABomb* pBomb = BombMatrix[subtractedIdx.Y][subtractedIdx.X];
	if (pBomb) return;

	pBomb = GetWorld()->SpawnActor<ABomb>();
	pBomb->Create(_realLoc, GlobalVar::BOMB_SIZE, _power, _bombType);
	BombMatrix[subtractedIdx.Y][subtractedIdx.X] = pBomb;
}

FIntPoint ABombManager::LocationToMatrixIdx(const FVector2D& _loc)
{
	FIntPoint idx = LocationToIndex(_loc - GetActorLocation());
	return idx;
}

FVector2D ABombManager::MatrixIdxToLocation(const FIntPoint& _idx)
{
	FVector2D loc = IndexToLocation(_idx) + GetActorLocation();
	return loc;
}

ABomb* ABombManager::GetBombRef(const FIntPoint& _idx)
{
	if (IsIndexOver(_idx) == true)
	{
		return nullptr;
	}
	return BombMatrix[_idx.Y][_idx.X];
}

ABomb* ABombManager::GetBombRef(const FVector2D& _realLoc)
{
	FIntPoint subtractedIdx = LocationToMatrixIdx(_realLoc);
	return GetBombRef(subtractedIdx);
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

std::vector<FIntPoint> ABombManager::GetExplodedIdxs(const FIntPoint& _idx, int _power)
{
	std::vector<FIntPoint> vec;
	vec.reserve(_power * 4 + 1);
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

FVector2D ABombManager::IndexToLocation(const FIntPoint& _Index)
{
	return FVector2D(_Index.X * TileSize.X, _Index.Y * TileSize.Y);
}

FIntPoint ABombManager::LocationToIndex(const FVector2D& _Location)
{
	FVector2D Location = _Location / TileSize;

	return FIntPoint(Location.iX(), Location.iY());
}