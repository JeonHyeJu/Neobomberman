#pragma once
#include <EngineCore/Actor.h>
#include "ContentsStruct.h"
#include "Matrix.h"

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

protected:
	 void BeginPlay() override;
	 void Tick(float _deltaTime) override;

private:
	void InitMap();
	void InitExplosionMatrix();
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
	SBombTailTypes GetBombTailTypes(const FIntPoint& _matIdx, EBombType _bombType, int _power);
	EBombTailType GetBombTailType(const FIntPoint& _nextIdx, bool* isEnd);
	std::vector<FIntPoint> GetBombRange(const FIntPoint& _matIdx, const SBombTailTypes& _tailInfo);
	void AppendExplodeTiles(const std::vector<FIntPoint>& _appendIdxs);

	void CheckExplodedBomb();
	void HandleExplodedBomb();
	void HandleExplodedBox();
	void ClearExplosionInfo();

	ATileMap* MapGround = nullptr;
	ATileMap* MapWall = nullptr;
	ATileMap* MapBox = nullptr;
	SBombMatrix ExplosionMatrix;

	std::list<class ABomb*> BombList;
	std::vector<class ABomb*> ConnectedBombList;
	std::vector<FIntPoint> ExplodeTileIdxs;
};
