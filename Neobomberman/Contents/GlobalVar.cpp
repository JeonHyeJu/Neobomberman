#include "PreCompile.h"
#include "GlobalVar.h"
#include <string>

const FVector2D GlobalVar::BOMBERMAN_SIZE = { 64, 64 };		// { 32, 32 }
const FVector2D GlobalVar::WINDOW_SIZE = { 608, 448 };		// { 304, 224 }
const FVector2D GlobalVar::BOMB_SIZE = { 32, 32 };			// { 16, 16 }
const FIntPoint GlobalVar::BATTLE_GROUND_COUNT = { 13, 11 };
const int GlobalVar::STAGE_H_MARGIN = 64;	// 32
const int GlobalVar::MAX_BOMB_CNT = 5;
const int GlobalVar::ORG_BOMB_POWER = 3;
const int GlobalVar::MAX_BOMB_POWER = 6;

const char* GlobalPath::ROOT = "Resources";
const char* GlobalPath::TILE = "Tiles";
const char* GlobalPath::DATA = "Data";
const char* GlobalPath::TILE_STAGE_1 = "TileStage_1";
const char* GlobalPath::TILE_STAGE_1_GUIDE = "TileStage_1_Guide";
const char* GlobalPath::OPENING = "Opening";

const char* GlobalPath::BACKGROUND = "Background";
const char* GlobalPath::CHARACTER = "Character_32x64";
const char* GlobalPath::RIDING = "Riding_32x32";
const char* GlobalPath::ENEMY = "Enemies_32x32";

const char* GlobalPath::MAP_WALL_DAT = "WallData.dat";
const char* GlobalPath::MAP_BOX_DAT = "BoxData.dat";

const char* GlobalPath::BOMB_ORG = "BombOrg_16x16";
const char* GlobalPath::BOMB_RED = "BombRed_16x16";