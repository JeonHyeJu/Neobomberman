#pragma once
#include <EngineCore/Actor.h>
#include <array>

#include "ContentsEnum.h"

enum BombState
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

	void InitOrgBomb(const FVector2D& _loc, int _power);
	void InitRedBomb(const FVector2D& _loc, int _power);

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	inline EBombType GetBombType() const
	{
		return BombType;
	}
	inline void SetBombType(const EBombType& _bombType)
	{
		BombType = _bombType;
	}
	inline void SetState(BombState _state)
	{
		State = _state;
	}

private:
	void InitSpriteCenter(std::string_view _spriteName, std::string_view _animName, std::string_view _explodeSpriteName);
	void InitExplodeSprites(int _power);
	void InitExplodeSpriteVector(std::string_view _spriteName, std::string_view _animName, std::vector<USpriteRenderer*>& _vector, const FVector2D& _loc);
	void InitExplodeSprite(USpriteRenderer** _spriteRenderer, std::string_view _spriteName, std::string_view _animName, const FVector2D& _loc);
	void Explode();
	void OnExploding();
	void OnEndAnimation();
	void _RunAnimHelper(std::vector<USpriteRenderer*>& _vec, std::string_view _animName, bool _isOff=true);

	EBombType BombType = EBombType::PLAIN;
	BombState State = BombState::None;

	std::string ExplodeAnimName = "";

	const char* TEMP_EXPLODE_SPRITE_NAME = "ExplodeCenter.png";
	const char* ANIM_RUNNING = "Bomb_Run";
	const char* ANIM_EXPLODE = "Bomb_Explode";

	const char* RESOURCE_PLAINBOMB_PATH = "BombOrg_16x16";	// 16x16
	const char* RESOURCE_REDBOMB_PATH = "BombRed_16x16";	// 16x16
	const float EXPLODE_SECONDS = 2.f;

	// Fixed size = GlobalVar::MAX_BOMB_POWER
	std::vector<USpriteRenderer*> ExplodeSprites_Up;
	std::vector<USpriteRenderer*> ExplodeSprites_Down;
	std::vector<USpriteRenderer*> ExplodeSprites_Left;
	std::vector<USpriteRenderer*> ExplodeSprites_Right;
	USpriteRenderer* ExplodeSprites_Center = nullptr;

	float AccumulatedSeconds = 0.f;
	int Power = 0;
};
