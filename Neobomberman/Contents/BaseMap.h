#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/PathFindAStar.h>
#include "ContentsEnum.h"

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

	void SetItems(const std::vector<EItem>& _list);
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
	bool CanMove(const FVector2D& _loc, bool _isPlayer=false);
	bool CanMove(const FIntPoint& _idx, bool _isPlayer=false);
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
	inline ATileMap* GetConverMap() const
	{
		return MapCover;
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
	FVector2D GetOrganizedLoc(const FVector2D& _loc);
	void BindExplodeEvent(std::function<void()> _fn)
	{
		ExplodeEvent = _fn;
	}
	const std::vector<FIntPoint>& GetSplashTileIdxs() const
	{
		return SplashTileIdxs;
	}
	bool IsInSplashWithVector(const FIntPoint& _pt, const std::vector<FIntPoint>& _vec)
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

	void RemoveItem(const FIntPoint& _idx);
	bool HasItem(const FIntPoint& _idx);
	EItem PopItem(const FIntPoint& _idx);

	/* Cheats */
	void CheatDestoyAllBoxes();

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
	ATileMap* MapCover = nullptr;

	FIntPoint PortalIdx = FIntPoint::NEGATIVE_ONE;

	std::vector<FIntPoint> SplashTileIdxs;

	std::function<void()> ExplodeEvent = nullptr;

	const std::list<FIntPoint> Temp;
};
