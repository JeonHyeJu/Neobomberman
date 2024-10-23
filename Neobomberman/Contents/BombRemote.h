#pragma once
#include "Bomb.h"

class BombRemote : public Bomb
{
public:
	BombRemote();
	~BombRemote();

	BombRemote(const BombRemote& _other) = delete;
	BombRemote(BombRemote&& _other) = delete;
	BombRemote& operator=(const BombRemote& _other) = delete;
	BombRemote& operator=(BombRemote&& _other) noexcept = delete;
};
