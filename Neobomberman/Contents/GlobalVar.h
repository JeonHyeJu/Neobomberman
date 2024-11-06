#pragma once
#include <EngineBase/EngineMath.h>

class GlobalVar
{
public:
	static const FVector2D WINDOW_SIZE;
	static const FIntPoint BATTLE_GROUND_COUNT;
	static const FVector2D BOMBERMAN_SIZE;
	static const FVector2D BOMB_SIZE;
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
	static const char* TILE_STAGE_1;
	static const char* TILE_STAGE_1_GUIDE;

	static const char* BACKGROUND;
	static const char* CHARACTER;
	static const char* RIDING;
	static const char* ENEMY;

	static const char* BOMB_ORG;
	static const char* BOMB_RED;

	static const char* MAP_WALL_DAT;
	static const char* MAP_BOX_DAT;

	// These are not static because only be used once or twice.
	inline std::string GetBackgroundPath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::BACKGROUND;
		return ret;
	}
	inline std::string GetCharacterPath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::CHARACTER;
		return ret;
	}
	inline std::string GetRidingPath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::RIDING;
		return ret;
	}
	inline std::string GetEnemyPath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::ENEMY;
		return ret;
	}
	inline std::string GetOpeningPath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::OPENING;
		return ret;
	}
	inline std::string GetOrgBombPath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::BOMB_ORG;
		return ret;
	}
	inline std::string GetRedBombPath() const
	{
		std::string ret = std::string(GlobalPath::ROOT) + "\\" + GlobalPath::BOMB_RED;
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