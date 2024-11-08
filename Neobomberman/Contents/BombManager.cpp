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

	CheckExplosion();
}

//void ABombManager::CheckExplosion()
//{
//	if (BombMatrix.size() == 0) return;
//
//	// TODO: Optimize
//	for (int y = 0; y < BombMatrix.size(); ++y)
//	{
//		for (int x = 0; x < BombMatrix[y].size(); ++x)
//		{
//			if (BombMatrix[y][x] == nullptr) continue;
//
//			if (BombMatrix[y][x]->State == BombState::Exploding)
//			{
//				BombMatrix[y][x]->SetState(BombState::FinishExploding);
//				std::vector<FIntPoint> explodeIdxs = GetExplodedIdxs(FIntPoint{ x, y }, BombMatrix[y][x]->Power);
//				AppendExplodeTiles(explodeIdxs);
//			}
//			else if (BombMatrix[y][x]->State == BombState::Over)
//			{
//				BombMatrix[y][x]->Destroy();
//				BombMatrix[y][x] = nullptr;
//			}
//		}
//	}
//}

//void ABombManager::Init(const FIntPoint& _Count, const FVector2D& _TileSize)
//{
//	TileCount = _Count;
//	TileSize = _TileSize;
//
//	BombMatrix.resize(_Count.Y);
//
//	for (size_t y = 0; y < BombMatrix.size(); y++)
//	{
//		BombMatrix[y].resize(_Count.X);
//	}
//}

void ABombManager::CheckExplosion()
{
	std::list<ABomb*>::iterator it = BombList.begin();
	std::list<ABomb*>::iterator itEnd = BombList.end();

	for (; it != itEnd; ++it)
	{
		ABomb* pBomb = *it;
		if (pBomb == nullptr) continue;

		if (pBomb->State == BombState::Exploding)
		{
			pBomb->SetState(BombState::FinishExploding);

			// Temp.. TODO!!
			for (size_t i = 0, size = pBomb->ExplodeIdxs.size(); i < size; ++i)
			{
				if (pBomb->ExplodeIdxs[i] == pBomb->MatrixIdx)
				{
					pBomb->ExplodeIntermediatly();
				}
			}
			AppendExplodeTiles(pBomb->ExplodeIdxs);
		}
		else if (pBomb->State == BombState::Over)
		{
			pBomb->Destroy();
			BombList.remove(pBomb);
			pBomb = nullptr;
		}
	}
}

void ABombManager::SetBomb(const FVector2D& _realLoc, const SBombTails& _tailInfo, EBombType _bombType, int _power)
{
	FIntPoint idx = LocationToMatrixIdx(_realLoc);

	FIntPoint realIdx = LocationToIndex(_realLoc);
	FVector2D orderedLoc = IndexToLocation(realIdx);

	std::list<ABomb*>::iterator it = BombList.begin();
	std::list<ABomb*>::iterator itEnd = BombList.end();

	for (; it != itEnd; ++it)
	{
		if (idx == (*it)->MatrixIdx)
		{
			// If a bomb already exists, don't make it.
			return;
		}
	}

	ABomb* pBomb = GetWorld()->SpawnActor<ABomb>();
	pBomb->Create(orderedLoc, idx, GlobalVar::BOMB_SIZE, _power, _bombType, _tailInfo);
	BombList.push_back(pBomb);
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