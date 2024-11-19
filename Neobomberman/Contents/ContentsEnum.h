#pragma once

enum class ERenderOrder
{
	COLMAP = -1000,
	BACKGROUND = -999,
	PORTAL = 9990,
	BOMB = 9997,
	CRUMBLING_BOX = 9998,
	MONSTER = 9999,
	PLAYER = 10000,
	UI = 19000,
	FADE = 20000,
	RESULT = 20001,
	RESULT_STAMP = 20002,
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

