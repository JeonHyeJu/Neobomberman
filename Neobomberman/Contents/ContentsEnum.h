#pragma once

enum class ERenderOrder
{
	COLMAP = -1000,
	BACKGROUND = -999,
	PLAYER = 10000,
	BOMB = 10001,
	UI = 20000,
};

enum class EBombType
{
	PLAIN = 0,
	RED,
};