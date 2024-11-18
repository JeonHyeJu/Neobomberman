#pragma once
#include <EngineBase/EngineMath.h>

class GlobalVar
{
public:
	static const FVector2D WINDOW_SIZE;
	static const FIntPoint BATTLE_GROUND_COUNT;
	static const FVector2D BOMBERMAN_SIZE;
	static const FVector2D BOMB_SIZE;
	static const FVector2D TIME_COUNT_SIZE;
	static const FVector2D TITLE_TIME_COUNT_SIZE;
	static const int STAGE_H_MARGIN;
	static const int MAX_BOMB_CNT;
	static const int ORG_BOMB_POWER;
	static const int MAX_BOMB_POWER;
};

class GlobalPath
{
public:
	static const char* ROOT;
	static const char* TILE;
	static const char* DATA;
	static const char* OPENING;
	static const char* EXPLODE;
	static const char* TILE_STAGE_1;
	static const char* TILE_STAGE_1_GUIDE;
	static const char* ANIM_CRUMBLING_BOX;

	static const char* BACKGROUND;
	static const char* CHARACTER;
	static const char* RIDING;
	static const char* ENEMY;

	static const char* BOMB_ORG;
	static const char* BOMB_RED;

	static const char* MAP_WALL_DAT;
	static const char* MAP_BOX_DAT;

	// These are not static because only be used once or twice.
	inline std::string GetAppendedRootPath(const char* _appendedStr) const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + _appendedStr;
		return ret;
	}
	inline std::string GetTileDataPath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::TILE + "\\" + GlobalPath::DATA;
		return ret;
	}
	inline std::string GetTileStage1Path() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::TILE + "\\" + GlobalPath::TILE_STAGE_1;
		return ret;
	}
	inline std::string GetTileStage1GuidePath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::TILE + "\\" + GlobalPath::TILE_STAGE_1_GUIDE;
		return ret;
	}
};