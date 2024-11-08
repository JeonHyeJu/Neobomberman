#pragma once
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include <vector>

struct SBombTailTypes
{
	std::vector<EBombTailType> Up;
	std::vector<EBombTailType> Down;
	std::vector<EBombTailType> Left;
	std::vector<EBombTailType> Right;

	SBombTailTypes()
	{
		Up.reserve(GlobalVar::MAX_BOMB_POWER);
		Down.reserve(GlobalVar::MAX_BOMB_POWER);
		Left.reserve(GlobalVar::MAX_BOMB_POWER);
		Right.reserve(GlobalVar::MAX_BOMB_POWER);
	}
};