#pragma once
#include <EngineCore/Actor.h>
#include <vector>
#include "ContentsEnum.h"

class APlayer : public AActor
{
public:
	struct PlayerAbility
	{
		int Power = 1;
		int Speed = 1;
		int BombCount = 1;
		bool HasGlove = false;
		bool HasShoes = false;
		bool HasMoveThrogh = false;
		bool IsRinding = false;
		EBombType BombType = EBombType::PLAIN;
	};

	APlayer();
	~APlayer();

	APlayer(const APlayer& _other) = delete;
	APlayer(APlayer&& _other) noexcept = delete;
	APlayer& operator=(const APlayer& _other) = delete;
	APlayer& operator=(APlayer&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	void RunSoundPlay();

	void LevelChangeStart();
	void LevelChangeEnd();

protected:

private:
	int CurIndex = 0;
	float Speed = 100.0f;
	const char* PLAYER_SPRITE_PATH = "MainCharater_White.png";	// 1024 x 640 (32x64)

	class USpriteRenderer* SpriteRendererHead = nullptr;
	class USpriteRenderer* SpriteRendererBody = nullptr;

	std::vector<class Bomb*> Bombs;
	PlayerAbility Ability;
};
