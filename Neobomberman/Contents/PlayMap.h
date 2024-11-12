#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/PathFindAStar.h>
#include "ContentsStruct.h"

class ATileMap;
class APlayMap : public AActor, public IPathFindData
{
public:
	APlayMap();
	~APlayMap();

	APlayMap(const APlayMap& _Other) = delete;
	APlayMap(APlayMap&& _Other) noexcept = delete;
	APlayMap& operator=(const APlayMap& _Other) = delete;
	APlayMap& operator=(APlayMap&& _Other) noexcept = delete;

	void InitMap();
	FVector2D GetPortalLoc() const;
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
	void SetPortalIdx(const FIntPoint& _idx)
	{
		PortalIdx = _idx;
	}

protected:
	 void BeginPlay() override;
	 void Tick(float _deltaTime) override;

private:
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
	void AppendSplash(const std::vector<FIntPoint>& _appendIdxs);

	void CheckLaunchedBomb();
	void RemoveExplodedBomb();
	void CheckExplodedBox();
	void ClearSplashArray();

	const int ENEMY_CNT_STAGE_1 = 4;

	ATileMap* MapGround = nullptr;
	ATileMap* MapWall = nullptr;
	ATileMap* MapBox = nullptr;

	std::vector<FIntPoint> SplashTileIdxs;

	FIntPoint PortalIdx = { 6, 10 };	// Init with first stage loc.
};
