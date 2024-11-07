#pragma once
#include <EngineCore/Actor.h>
#include "ContentsEnum.h"

class ABomb;
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
	void SetBomb(const FVector2D& _Location, EBombType _bombType, int _power);

	FIntPoint LocationToMatrixIdx(const FVector2D& _loc);
	FVector2D MatrixIdxToLocation(const FIntPoint& _idx);
	bool IsIndexOver(const FIntPoint& _Index);

	ABomb* GetBombRef(const FIntPoint& _Index);
	ABomb* GetBombRef(const FVector2D& _Location);
	
	inline const std::vector<FIntPoint>& GetExplodedTileIdxs() const
	{
		return ExplodeTileIdxs;
	}
	inline bool HasExplodedBomb() const
	{
		bool ret = (ExplodeTileIdxs.size() > 0);
		return ret;
	}
	inline void ClearExplodeTileIdxs()
	{
		ExplodeTileIdxs.clear();
	}

protected:
	void Tick(float _deltaTime) override;

private:
	std::vector<FIntPoint> GetExplodedIdxs(const FIntPoint& _idx, int _power);
	void AppendExplodeTiles(const std::vector<FIntPoint>& _vec);
	FVector2D IndexToLocation(const FIntPoint& _Index);
	FIntPoint LocationToIndex(const FVector2D& _Location);

	FIntPoint TileCount;
	FVector2D TileSize;
	std::vector<std::vector<ABomb*>> BombMatrix;
	std::vector<FIntPoint> ExplodeTileIdxs;
};
