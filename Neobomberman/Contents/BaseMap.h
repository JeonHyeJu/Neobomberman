#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/PathFindAStar.h>

class ATileMap;
class ABaseMap : public AActor, public IPathFindData
{
public:
	ABaseMap();
	~ABaseMap();

	ABaseMap(const ABaseMap& _other) = delete;
	ABaseMap(ABaseMap&& _other) noexcept = delete;
	ABaseMap& operator=(const ABaseMap& _other) = delete;
	ABaseMap& operator=(ABaseMap&& _other) noexcept = delete;

	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
	bool CanMove(const FVector2D& _loc);
	bool CanMove(const FIntPoint& _idx);
	bool IsMove(const FIntPoint& _Point) override;

	inline ATileMap* GetGroundMap() const
	{
		return MapGround;
	}
	inline ATileMap* GetWallMap() const
	{
		return MapWall;
	}
	inline ATileMap* GetBoxMap() const
	{
		return MapBox;
	}

	inline void SetPortalIdx(const FIntPoint& _idx)
	{
		PortalIdx = _idx;
	}
	inline const FIntPoint& GetPortalIdx() const
	{
		return PortalIdx;
	}
	bool GetIsPortalOpened() const;
	FVector2D GetPortalLoc() const;
	void OpenPortal();
	FIntPoint LocationToMatrixIdx(const FVector2D& _loc);
	FVector2D MatrixIdxToLocation(const FIntPoint& _idx);
	void BindExplodeEvent(std::function<void()> _fn)
	{
		ExplodeEvent = _fn;
	}
	const std::vector<FIntPoint>& GetSplashTileIdxs() const
	{
		return SplashTileIdxs;
	}
	bool IsInSplash(const FIntPoint& _pt)
	{
		for (size_t i = 0, size = SplashTileIdxs.size(); i < size; ++i)
		{
			if (_pt == SplashTileIdxs[i])
			{
				return true;
			}
		}
		return false;
	}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;

	void AppendSplash(const std::vector<FIntPoint>& _appendIdxs);
	void CheckLaunchedBomb();
	void RemoveExplodedBomb();
	void CheckExplodedBox();
	void ClearSplashArray();
	
	ATileMap* MapGround = nullptr;
	ATileMap* MapWall = nullptr;
	ATileMap* MapBox = nullptr;

	FIntPoint PortalIdx = FIntPoint::NEGATIVE_ONE;

	std::vector<FIntPoint> SplashTileIdxs;

	std::function<void()> ExplodeEvent = nullptr;
};
