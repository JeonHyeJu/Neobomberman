#pragma once

enum class ERenderOrder
{
	COLMAP = -1000,
	BACKGROUND = -999,
	BOMB = 9998,
	CRUMBLING_BOX = 9999,
	PLAYER = 10000,
	UI = 20000,
};

enum class EBombType
{
	PLAIN = 0,
	RED,
};