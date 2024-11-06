#pragma once
#include <EngineCore/Actor.h>
#include "Bomb.h"

class ABombManager : public AActor
{
public:
	ABombManager();
	~ABombManager();

	ABombManager(const ABombManager& _other) = delete;
	ABombManager(ABombManager&& _other) noexcept = delete;
	ABombManager& operator=(const ABombManager& _other) = delete;
	ABombManager& operator=(ABombManager&& _other) noexcept = delete;

	void Init(const FIntPoint& _Count, const FVector2D& _TileSize);
	void SetBomb(const FIntPoint& _Index, EBombType _bombType, int _power);
	void SetBomb(const FVector2D& _Location, EBombType _bombType, int _power);
	FVector2D IndexToLocation(const FIntPoint& _Index);
	FIntPoint LocationToIndex(const FVector2D& _Location);
	ABomb* GetBombRef(const FIntPoint& _Index);
	ABomb* GetBombRef(const FVector2D& _Location);
	bool IsIndexOver(const FIntPoint& _Index);

	bool HasExplodedBomb();
	inline const std::vector<FIntPoint>& GetExplodedTileIdxs() const
	{
		return ExplodeTileIdxs;
	}
	void ClearExplodeTileIdxs();

protected:
	void Tick(float _deltaTime) override;

private:
	void AddExplodeTileIdxs(const std::vector<FVector2D>& _locs);

	FIntPoint TileCount;
	FVector2D TileSize;
	std::vector<std::vector<ABomb*>> BombMatrix;
	std::vector<FIntPoint> ExplodeTileIdxs;
};
