#pragma once
#include <EngineCore/Actor.h>

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

protected:
	 void BeginPlay() override;

private:
	void InitMap();
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);

	ATileMap* MapGround = nullptr;
	ATileMap* MapWall = nullptr;
	ATileMap* MapBox = nullptr;
};
