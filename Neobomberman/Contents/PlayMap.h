#pragma once
#include <EngineCore/Actor.h>
#include "ContentsStruct.h"

class ATileMap;
class APlayMap : public AActor
{
public:
	APlayMap();
	~APlayMap();

	APlayMap(const APlayMap& _Other) = delete;
	APlayMap(APlayMap&& _Other) noexcept = delete;
	APlayMap& operator=(const APlayMap& _Other) = delete;
	APlayMap& operator=(APlayMap&& _Other) noexcept = delete;

	inline ATileMap* GetWallMap() const
	{
		return MapWall;
	}

	inline ATileMap* GetBoxMap() const
	{
		return MapBox;
	}
	
	void SetBomb(const FVector2D& _loc, EBombType _bombType, int _power);
	void SetPortalLoc(const FIntPoint& _idx)
	{
		PortalIdx = _idx;
	}

protected:
	 void BeginPlay() override;
	 void Tick(float _deltaTime) override;

private:
	void InitMap();

	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
	SBombTailTypes GetBombTailTypes(const FIntPoint& _matIdx, EBombType _bombType, int _power);
	EBombTailType GetBombTailType(const FIntPoint& _nextIdx, bool* _isEnd, bool _isLast);
	std::vector<FIntPoint> GetBombRange(const FIntPoint& _matIdx, const SBombTailTypes& _tailInfo);
	void AppendSplash(const std::vector<FIntPoint>& _appendIdxs);

	void CheckLaunchedBomb();
	void RemoveExplodedBomb();
	void CheckExplodedBox();
	void ClearSplashArray();

	const int ENEMY_CNT_STAGE_1 = 4;

	ATileMap* MapGround = nullptr;
	ATileMap* MapWall = nullptr;
	ATileMap* MapBox = nullptr;

	std::list<class ABomb*> BombList;
	std::vector<FIntPoint> SplashTileIdxs;

	FIntPoint PortalIdx = { 6, 10 };	// Init with first stage loc.
};
