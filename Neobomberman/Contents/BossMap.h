#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/PathFindAStar.h>
#include "ContentsStruct.h"

class ATileMap;
class ABossMap : public AActor
{
public:
	ABossMap();
	~ABossMap();

	ABossMap(const ABossMap& _other) = delete;
	ABossMap(ABossMap&& _other) noexcept = delete;
	ABossMap& operator=(const ABossMap& _other) = delete;
	ABossMap& operator=(ABossMap&& _other) noexcept = delete;

	void InitMap();

	inline ATileMap* GetGroundMap() const
	{
		return MapGround;
	}
	inline ATileMap* GetWallMap() const
	{
		return MapWall;
	}

	ATileMap* MapGround = nullptr;
	ATileMap* MapWall = nullptr;

private:
	bool Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName);
};
