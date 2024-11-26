#pragma once
#include <EngineCore/Actor.h>
#include <EngineBase/FSMStateManager.h>
#include <EnginePlatform/EngineSound.h>
#include <vector>
#include "ContentsEnum.h"

enum class EPlayerBlinkAndColState
{
	BLINK_ON_COL_OFF = 0,	// Blink on, collision off
	BLINK_OFF_COL_ON,
};

enum class EPlayerState
{
	REBORN = 0,
	IDLE,
	MOVE,
	DEAD,
	PORTAL,
};

class APlayer : public AActor
{
public:
	struct SAnimKeepInfo
	{
		float Seconds = 0.f;
		float AnimSeconds = 1.5f;
		float WaitSeconds = 5.f;
		bool IsRunning = false;
	};

	struct SPlayerAbility
	{
		int Power = 2;
		float Speed = 100.f;
		int BombCount = 5;	// Temp
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
	void LevelChangeStart() override;

	inline void SetCurMap(class ABaseMap* _map)
	{
		CurMapPtr = _map;
	}
	void SetGameUI(class AGameUI* _ptr)
	{
		GameUIPtr = _ptr;
	}
	inline bool GetIsClear() const
	{
		return IsClear;
	}
	inline bool GetIsDead() const
	{
		return IsDead;
	}

	void Kill();

protected:
	void OnResume() override;

private:
	void InitSounds();

	std::string GetDirectionStr();
	void DropBomb();
	void Reborn();

	bool IsDownAnyKeyWithSetDir();
	bool IsPressedAnyKey();

	void OnEnterCollision(class AActor* _actor);
	void OnEndPortalAnim();

	void OnReborn();
	void OnIdle();
	void OnDead();
	void OnShift();

	void OnTurnOnBlink();
	void OnTurnOffBlink();

	void Idling(float _deltaTime);
	void Moving(float _deltaTime);
	void Dying(float _deltaTime);
	void Blinking(float _deltaTime);

	void OnEndFadeOut();

	int CurIndex = 0;
	const char* PLAYER_SPRITE_PATH = "MainCharater_White.png";	// 1024 x 640 (32x64)

	class USpriteRenderer* SpriteRendererHead = nullptr;
	class USpriteRenderer* SpriteRendererBody = nullptr;

	//class UEngineWinImage* CollisionImage = nullptr;

	class U2DCollision* Collision = nullptr;

	ABaseMap* CurMapPtr = nullptr;
	const int BLINK_SECONDS = 1;

	SPlayerAbility Ability;

	UFSMStateManager Fsm;
	UFSMStateManager FsmH;

	FVector2D Direction;
	SAnimKeepInfo BlinkEyeAnimInfo;
	SAnimKeepInfo DyingAnimInfo;

	AGameUI* GameUIPtr = nullptr;

	int Score = 600;	// Temp
	bool IsClear = false;
	bool IsDead = false;

	const float BLINK_SECS = 2.f;
};
