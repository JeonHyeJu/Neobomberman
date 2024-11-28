#pragma once
#include <EngineCore/Actor.h>
#include <EngineBase/FSMStateManager.h>
#include "ContentsStruct.h"

enum class EBombState
{
	Running,
	Launched,
	WaitAnim,
	Exploding,
	Over
};

class ATileMap;
class ABaseMap;
class USpriteRenderer;
class ABomb : public AActor
{
	friend class ABaseMap;

public:
	ABomb();
	~ABomb();

	ABomb(const ABomb& _other) = delete;
	ABomb(ABomb&& _other) noexcept = delete;
	ABomb& operator=(const ABomb& _other) = delete;
	ABomb& operator=(ABomb&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	void Init(const FVector2D& _loc, EBombType _bombType, int _power, ABaseMap* _curMap, ERenderOrder _order);
	void InitSpriteAndAnim(const SBombTailTypes& _tailInfo);
	void ExplodeBySplash();
	void SetCurMap(ABaseMap* _map)
	{
		CurMapPtr = _map;
	}
	inline EBombType GetBombType() const
	{
		return BombType;
	}
	inline EBombState GetState() const
	{
		return static_cast<EBombState>(Fsm.GetState());
	}
	bool IsInSplash(const std::vector<FIntPoint>& _splashVec)
	{
		for (size_t i = 0, size = _splashVec.size(); i < size; ++i)
		{
			if (MatrixIdx == _splashVec[i])
			{
				return true;
			}
		}
		return false;
	}
	inline FIntPoint GetMatrixIdx() const
	{
		return MatrixIdx;
	}
	inline void SetIsMovableForPlayer(bool _val)
	{
		IsMovableForPlayer = _val;
	}
	inline bool GetIsMovableForPlayer() const
	{
		return IsMovableForPlayer;
	}

	/* Static function */
	static bool CanSetBombThisIdx(const FIntPoint& _idx)
	{
		std::list<ABomb*>::iterator it = ABomb::BombList.begin();
		std::list<ABomb*>::iterator itEnd = ABomb::BombList.end();

		for (; it != itEnd; ++it)
		{
			if (_idx == (*it)->MatrixIdx)
			{
				// If a bomb already exists, don't make it.
				return false;
			}
		}

		return true;
	}
	static int GetBombCnt()
	{
		return static_cast<int>(BombList.size());
	}

private:
	void CreateTail(const char* _img, const char* _imgMid, const char* _animName, const FVector2D& loc, const std::vector<EBombTailType>& _tails, std::vector<USpriteRenderer*>* _pAnimVec);
	void InitSpriteCenter(std::string_view _spriteName, std::string_view _animName, std::string_view _explodeSpriteName);
	void _InitDefaultSprite4D(std::string_view _spriteName, std::string_view _animName, std::vector<USpriteRenderer*>& _vector, const FVector2D& _loc);
	void _InitDefaultSprite(USpriteRenderer** _spriteRenderer, std::string_view _spriteName, std::string_view _animName, const FVector2D& _animLoc);

	SBombTailTypes GetBombTailTypes(const FIntPoint& _matIdx, EBombType _bombType, int _power);
	EBombTailType GetBombTailType(const FIntPoint& _nextIdx, bool* _isEnd, bool _isLast);		// Temp. intergrate two tile map.
	std::vector<FIntPoint> GetBombRange(const FIntPoint& _matIdx, const SBombTailTypes& _tailInfo);

	void Running(float _deltaTime);
	void OnLaunched();
	void OnStartAnimation();
	void OnEndAnimation();

	void RunExplosionAnim(bool _isOn);
	void _RunAnimHelper(USpriteRenderer* _centerSprite, std::string_view _animName, bool _isOn=true);
	void _RunAnimHelper(std::vector<USpriteRenderer*>& _vec, std::string_view _animName, bool _isOff=true);

	static std::list<ABomb*> BombList;

	ERenderOrder Order;

	UFSMStateManager Fsm;
	ABaseMap* CurMapPtr = nullptr;
	class U2DCollision* Collision = nullptr;	// For checking overlaped position with palyer.

	EBombType BombType = EBombType::PLAIN;

	const float EXPLODE_SECONDS = 2.f;
	float AccumulatedSeconds = 0.f;
	int Power = 0;
	FVector2D Size;
	FIntPoint MatrixIdx;
	SBombTailTypes BombTails;
	std::vector<FIntPoint> ExplodeIdxs;

	FVRect VertexPts;
	bool IsMovableForPlayer = true;

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

	const char* ANIM_BOMB_RUNNING = "Bomb_Run";
	const char* ANIM_EXPLODE_CENTER = "ExplodeCenter";
	const char* ANIM_EXPLODE_UP = "ExplodeUp";
	const char* ANIM_EXPLODE_DOWN = "ExplodeDown";
	const char* ANIM_EXPLODE_LEFT = "ExplodeLeft";
	const char* ANIM_EXPLODE_RIGHT = "ExplodeRight";

	/* Sounds */
	const char* SFXExplosion = "Explosion.mp3";
};
