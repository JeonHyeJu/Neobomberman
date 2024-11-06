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

	for (int i = 0; i < BombMatrix.size(); ++i)
	{
		for (int j = 0; j < BombMatrix[i].size(); ++j)
		{
			if (BombMatrix[i][j] == nullptr) continue;

			if (BombMatrix[i][j]->State == BombState::Over)
			{
				BombMatrix[i][j]->Destroy();
				BombMatrix[i][j] = nullptr;
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

void ABombManager::SetBomb(const FIntPoint& _idx, EBombType _bombType, int _power)
{
	if (IsIndexOver(_idx) == true)
	{
		return;
	}

	ABomb* pBomb = BombMatrix[_idx.Y][_idx.X];
	if (pBomb) return;

	pBomb = GetWorld()->SpawnActor<ABomb>();
	
	if (_bombType == EBombType::RED)
	{
		pBomb->InitRedBomb(_power);
	}
	else // EBombType::PLAIN
	{
		pBomb->InitOrgBomb(_power);
	}

	FVector2D loc = IndexToLocation(_idx);
	pBomb->SetActorLocation(loc);
	BombMatrix[_idx.Y][_idx.X] = pBomb;
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