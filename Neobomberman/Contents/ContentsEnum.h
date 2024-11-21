#pragma once

enum class ERenderOrder
{
	COLMAP = -1000,
	BACKGROUND = -999,
	BACKGROUND_OVER = -998,
	CUT_SCENE_BG = 1,
	CUT_SPACESHIP = 2,
	CUT_SCENE = 3,
	CUT_LETTER_BOX = 4,
	OPENING = 5,
	OPENING_STRING = 6,
	PORTAL = 9990,
	BOMB = 9997,
	CRUMBLING_BOX = 9998,
	MONSTER = 9999,
	PLAYER = 10000,
	UI = 19000,
	UI_PLUS = 19001,
	FADE = 20000,
	UI_OVER_FADE = 20001,
	RESULT = 20002,
	RESULT_STAMP = 20003,
};

enum class EBombType
{
	PLAIN = 0,
	RED,
};

enum class EBombTailType
{
	NONE = 0,
	END,
	CONNECT
};

enum class EDirectionType
{
	NONE = 0,
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

enum class ECollisionGroup
{
	PlayerBody,
	MonsterBody,
};

