#pragma once
#include <EngineCore/Actor.h>
#include "ContentsEnum.h"

enum class BombState
{
	None = 0,
	Running,
	StartExploding,
	Exploding,
	FinishExploding,
	Over
};

class USpriteRenderer;
class ABomb : public AActor
{
	friend class ABombManager;

public:
	ABomb();
	~ABomb();

	ABomb(const ABomb& _other) = delete;
	ABomb(ABomb&& _other) noexcept = delete;
	ABomb& operator=(const ABomb& _other) = delete;
	ABomb& operator=(ABomb&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	void Create(const FVector2D& _loc, const FVector2D& _size, int _power, EBombType _type);
	inline EBombType GetBombType() const
	{
		return BombType;
	}
	inline BombState GetState() const
	{
		return State;
	}
	inline void SetState(BombState _state)
	{
		State = _state;
	}

private:
	void InitSpriteCenter(std::string_view _spriteName, std::string_view _animName, std::string_view _explodeSpriteName);
	void InitSprite4Direction();
	void _InitDefaultSprite4D(std::string_view _spriteName, std::string_view _animName, std::vector<USpriteRenderer*>& _vector, const FVector2D& _loc);
	void _InitDefaultSprite(USpriteRenderer** _spriteRenderer, std::string_view _spriteName, std::string_view _animName, const FVector2D& _animLoc);

	void Explode();

	void _RunAnimHelper(USpriteRenderer* _centerSprite, std::string_view _animName, bool _isOn=true);
	void _RunAnimHelper(std::vector<USpriteRenderer*>& _vec, std::string_view _animName, bool _isOff=true);

	void OnExploding();
	void OnEndAnimation();

	EBombType BombType = EBombType::PLAIN;
	BombState State = BombState::None;

	const float EXPLODE_SECONDS = 2.f;
	float AccumulatedSeconds = 0.f;
	int Power = 0;
	FVector2D Size;

	USpriteRenderer* ExplodeSprite_Center = nullptr;
	std::vector<USpriteRenderer*> ExplodeSprites_Up;
	std::vector<USpriteRenderer*> ExplodeSprites_Down;
	std::vector<USpriteRenderer*> ExplodeSprites_Left;
	std::vector<USpriteRenderer*> ExplodeSprites_Right;

	const char* IMG_EXPLOSION_CENTER = "ExplodeCenter.png";
	const char* IMG_EXPLOSION_UP = "ExplodeUp.png";
	const char* IMG_EXPLOSION_DOWN = "ExplodeDown.png";
	const char* IMG_EXPLOSION_LEFT = "ExplodeLeft.png";
	const char* IMG_EXPLOSION_RIGHT = "ExplodeRight.png";
	const char* IMG_EXPLOSION_UPMID = "ExplodeUpMid.png";
	const char* IMG_EXPLOSION_DOWNMID = "ExplodeDownMid.png";
	const char* IMG_EXPLOSION_LEFTMID = "ExplodeLeftMid.png";
	const char* IMG_EXPLOSION_RIGHTMID = "ExplodeRightMid.png";

	const char* ANIM_RUNNING = "Bomb_Run";
	const char* ANIM_EXPLODE_CENTER = "ExplodeCenter";
	const char* ANIM_EXPLODE_UP = "ExplodeUp";
	const char* ANIM_EXPLODE_DOWN = "ExplodeDown";
	const char* ANIM_EXPLODE_LEFT = "ExplodeLeft";
	const char* ANIM_EXPLODE_RIGHT = "ExplodeRight";
};
