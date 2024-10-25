#pragma once
#include "Bomb.h"

class BombPlain : public Bomb
{
public:
	BombPlain();
	~BombPlain();

	BombPlain(const BombPlain& _other) = delete;
	BombPlain(BombPlain&& _other) noexcept = delete;
	BombPlain& operator=(const BombPlain& _other) = delete;
	BombPlain& operator=(BombPlain&& _other) noexcept = delete;
};
