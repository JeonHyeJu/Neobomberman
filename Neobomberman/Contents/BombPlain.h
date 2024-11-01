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

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

private:
	int CurIndex = 0;
	float AccumulatedSeconds = 0.f;

	const char* RESOURCE_PATH = "BombOrg_16x16";	// 16x16
	class USpriteRenderer* SpriteRenderer = nullptr;
};
