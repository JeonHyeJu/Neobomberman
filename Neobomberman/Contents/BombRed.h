#pragma once
#include "Bomb.h"

class BombRed : public Bomb
{
public:
	BombRed();
	~BombRed();

	BombRed(const BombRed& _other) = delete;
	BombRed(BombRed&& _other) noexcept = delete;
	BombRed& operator=(const BombRed& _other) = delete;
	BombRed& operator=(BombRed&& _other) noexcept = delete;
};
