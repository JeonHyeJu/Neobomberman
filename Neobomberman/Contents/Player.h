#pragma once
#include <EngineCore/Actor.h>

class APlayer : public AActor
{
public:
	APlayer();
	~APlayer();

	APlayer(const APlayer& _other) = delete;
	APlayer(APlayer&& _other) noexcept = delete;
	APlayer& operator=(const APlayer& _other) = delete;
	APlayer& operator=(APlayer&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

protected:

private:
	int CurIndex = 0;
	float Speed = 100.0f;
	const char* PLAYER_SPRITE_PATH = "MainCharater_White.png";	// 1024 x 640 (32x64)
	class USpriteRenderer* SpriteRendererHead = nullptr;
	class USpriteRenderer* SpriteRendererBody = nullptr;
};
