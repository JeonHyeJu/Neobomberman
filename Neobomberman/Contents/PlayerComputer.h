#pragma once
#include <EngineCore/Actor.h>
#include <EngineBase/FSMStateManager.h>
#include "ContentsEnum.h"

class APlayerComputer : public AActor
{
public:
	enum class EPlayerComputerState
	{
		REBORN = 0,
		IDLE,
		THINKING,
		MOVE,
		DEAD,
		END,
	};

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
		float Speed = 1;
		int BombCount = 1;
		bool HasGlove = false;
		bool HasShoes = false;
		bool HasMoveThrogh = false;
		bool IsRinding = false;
		EBombType BombType = EBombType::PLAIN;
	};

	APlayerComputer();
	~APlayerComputer();

	APlayerComputer(const APlayerComputer& _other) = delete;
	APlayerComputer(APlayerComputer&& _other) noexcept = delete;
	APlayerComputer& operator=(const APlayerComputer& _other) = delete;
	APlayerComputer& operator=(APlayerComputer&& _other) noexcept = delete;

	/* Setter */
	void SetCurMap(class ABaseMap* _map);
	void SetFirstDestination(const FIntPoint& _idx)
	{
		FirstIdx = _idx;
	}
	void SetStartDelay(float _seconds)
	{
		StartDelayMs = _seconds;
	}

	void InitSprite(std::string_view _sprite="");
	void SetStartLoc(const FVector2D& _val)
	{
		StartLocation = _val;
	}
	inline bool GetIsDead() const
	{
		return IsDead;
	}
	inline bool GetCanHit() const
	{
		return CanHit;
	}
	inline const FVector2D& GetDamageSize() const
	{
		return DamageSize;
	}
	inline const URect& GetDamageMargin() const
	{
		return DamageMargin;
	}

	void Kill();

	static void SetSpritePath(std::string_view _sprite)
	{
		SpritePath = _sprite;
	}

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	std::string GetDirectionStr();
	void Reborn();
	void ResetDroppedBombs();

	/* Fsm start callbacks */
	void OnReborn();
	void OnIdle();
	void OnMove();
	void OnDead();
	void OnEnd();

	/* Fsm update callbacks */
	void Idling(float _deltaTime);
	void Thinking(float _deltaTime);
	void Moving(float _deltaTime);
	void Dying(float _deltaTime);

	/* FsmH start callbacks */
	void OnTurnOnBlink();
	void OnTurnOffBlink();

	/* FsmH update callbacks */
	void Blinking(float _deltaTime);

	class USpriteRenderer* SpriteRendererHead = nullptr;
	class USpriteRenderer* SpriteRendererBody = nullptr;

	class ABaseMap* MapPtr = nullptr;

	FVector2D StartLocation;

	bool CanHit = true;
	FVector2D DamageSize;
	URect DamageMargin = URect{ 0, 0, 0, 0 };

	/* Initialization info */
	float StartDelayMs = 0.f;
	FIntPoint FirstIdx;

	UFSMStateManager Fsm;
	UFSMStateManager FsmH;

	FVector2D Direction;
	SAnimKeepInfo BlinkEyeAnimInfo;
	SAnimKeepInfo DyingAnimInfo;

	bool IsDead = false;
	bool IsImmotal = false;

	const float BLINK_SECS = 2.f;
	std::list<class ABomb*> DroppedBombs;

	int MoveOrder[4] = { 0, 0, 0, 0 };
	const float DEFAULT_SPEED = 75.f;

	SPlayerAbility Ability;

	FIntPoint Destination = FIntPoint::NEGATIVE_ONE;
	FIntPoint PPrevRouteIdx;
	FIntPoint PrevRouteIdx;

	static std::string SpritePath;

	/* Sounds */
	const char* SFXDropBomb = "CreateBomb.mp3";
	const char* SFXDying = "Dying.mp3";
	const char* SFXWalking = "Walking.mp3";
	const char* SFXItem = "Item.mp3";
};
