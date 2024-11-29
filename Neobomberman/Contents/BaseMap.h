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

	void Initialize()
	{
		InitSprite();
		InitTileMap();
	}
	void Initialize(const std::vector<EItem>& _items)
	{
		Items = _items;
		InitSprite();
		InitTileMap();
	}
	void Initialize(const std::vector<EItem>& _items, const FIntPoint& _portalIdx)
	{
		Items = _items;
		PortalIdx = _portalIdx;

		InitSprite();
		InitTileMap();
	}

	inline ATileMap* GetWallMap() const
	{
		return MapWall;
	}
	inline ATileMap* GetBoxMap() const
	{
		return MapBox;
	}

	/* Move */
	bool CanMove(const FVector2D& _loc, bool _isPlayer=false);
	bool IsMove(const FIntPoint& _Point) override;

	/* Portal */
	void OpenPortal();
	FVector2D GetPortalLoc() const;
	bool GetIsPortalOpened() const;
	inline const FIntPoint& GetPortalIdx() const
	{
		return PortalIdx;
	}
	
	/* Util */
	FIntPoint LocationToIndex(const FVector2D& _loc);
	FVector2D IndexToLocation(const FIntPoint& _idx);
	FIntPoint LocationToMatrixIdx(const FVector2D& _loc);
	FVector2D MatrixIdxToLocation(const FIntPoint& _idx);
	FVector2D GetOrganizedLoc(const FVector2D& _loc);

	/* Bomb */
	inline void BindExplodeEvent(std::function<void()> _fn)
	{
		ExplodeEvent = _fn;
	}
	inline const std::vector<FIntPoint>& GetSplashTileIdxs() const
	{
		return SplashTileIdxs;
	}
	bool IsInSplash(const FIntPoint& _pt);
	bool IsInSplashWithVector(const FIntPoint& _pt, const std::vector<FIntPoint>& _vec);

	/* Item */
	bool HasShowingItem(const FIntPoint& _idx);
	bool HasItem(const FIntPoint& _idx);
	EItem PopItem(const FIntPoint& _idx);

	/* Cheats */
	void CheatDestoyAllBoxes();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;

	virtual void InitSprite() = 0;
	virtual void InitTileMap() = 0;

	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);

	/* Bomb */
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
	std::vector<EItem> Items;
};
