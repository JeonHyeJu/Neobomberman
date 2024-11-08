#pragma once
#include <EngineCore/Actor.h>
#include "ContentsEnum.h"

class ATileMap;
class ABombManager;
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
	
	inline ABombManager* GetBombManager() const
	{
		return BombManager;
	}
	void SetBomb(const FVector2D& _loc, EBombType _bombType, int _power);

protected:
	 void BeginPlay() override;
	 void Tick(float _deltaTime) override;

private:
	void InitMap(const FIntPoint& _tileIdxs, const FVector2D& _tileSize, const FIntPoint& _moveLoc);
	void InitBombManager(const FIntPoint& _tileIdxs, const FVector2D& _tileSize, const FIntPoint& _moveLoc);
	void checkExplodedBombs();
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
	void _AddExplosion(const FIntPoint& _orgIdx, const FIntPoint& direc, int _idx, std::vector<EBombTailType>& _vec, bool* isUpEnd);

	ATileMap* MapGround = nullptr;
	ATileMap* MapWall = nullptr;
	ATileMap* MapBox = nullptr;
	ABombManager* BombManager = nullptr;
};
