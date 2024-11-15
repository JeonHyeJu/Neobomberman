#pragma once
#include <EngineCore/Actor.h>
#include <EngineBase/FSMStateManager.h>
#include <EnginePlatform/EngineSound.h>
#include <vector>
#include "ContentsEnum.h"

enum class EPlayerState
{
	REBORN = 0,
	IDLE,
	MOVE,
	DEAD,
};

class APlayer : public AActor
{
public:
	struct SAnimKeepInfo
	{
		float Seconds = 0.f;
		float AnimSeconds = 1.f;
		float WaitSeconds = 3.f;
		bool IsRunning = false;
	};

	struct SPlayerAbility
	{
		int Power = 2;
		float Speed = 100.f;
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

	inline void SetCurMap(class APlayMap* _map)
	{
		CurMap = _map;
	}

protected:

private:
	void InitSounds();

	std::string GetDirectionStr();
	void DropBomb();

	bool IsDownAnyKeyWithSetDir();
	bool IsPressedAnyKey();

	void OnEnterCollision(class AActor* _actor);
	void OnReborn();
	void OnIdle();
	void OnDead();

	void Reborning(float _deltaTime);
	void Idling(float _deltaTime);
	void Moving(float _deltaTime);
	void Dying(float _deltaTime);

	int CurIndex = 0;
	const char* PLAYER_SPRITE_PATH = "MainCharater_White.png";	// 1024 x 640 (32x64)

	class USpriteRenderer* SpriteRendererHead = nullptr;
	class USpriteRenderer* SpriteRendererBody = nullptr;

	//class UEngineWinImage* CollisionImage = nullptr;

	class U2DCollision* Collision = nullptr;

	class APlayMap* CurMap = nullptr;
	const int BLINK_SECONDS = 1;

	SPlayerAbility Ability;

	UFSMStateManager FsmH;

	FVector2D Direction;
	SAnimKeepInfo BlinkEyeAnimInfo;
	SAnimKeepInfo DyingAnimInfo;
};
