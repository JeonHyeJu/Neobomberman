#pragma once
#include <EngineCore/Actor.h>

class ATileMap;
class ABaseMap : public AActor
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
	FVector2D GetPortalLoc() const;
	void OpenPortal();
	FIntPoint LocationToMatrixIdx(const FVector2D& _loc);
	FVector2D MatrixIdxToLocation(const FIntPoint& _idx);

	// Temp
	inline bool GetIsMovablePortal(const FIntPoint& _Point) const
	{
		bool isPortal = _Point == FIntPoint({ 6, 10 });
		return !isPortal;
	}
	bool GetIsPortalOpened() const
	{
		return IsPortalOpened;
	}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;
	virtual void LevelChangeStart() override;

	void AppendSplash(const std::vector<FIntPoint>& _appendIdxs);
	void CheckLaunchedBomb();
	void RemoveExplodedBomb();
	void CheckExplodedBox();
	void ClearSplashArray();
	
	ATileMap* MapGround = nullptr;
	ATileMap* MapWall = nullptr;
	ATileMap* MapBox = nullptr;

	FIntPoint PortalIdx = FIntPoint::NEGATIVE_ONE;
	bool IsPortalOpened = false;

	std::vector<FIntPoint> SplashTileIdxs;
};
