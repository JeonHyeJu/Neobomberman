
#include "PreCompile.h"
#include "Bomb.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"

#include <EngineCore/SpriteRenderer.h>
#include <EngineBase/EngineDebug.h>
#include <functional>
#include <algorithm>

ABomb::ABomb()
{
	ExplodeSprites_Up.reserve(GlobalVar::MAX_BOMB_POWER);
	ExplodeSprites_Down.reserve(GlobalVar::MAX_BOMB_POWER);
	ExplodeSprites_Left.reserve(GlobalVar::MAX_BOMB_POWER);
	ExplodeSprites_Right.reserve(GlobalVar::MAX_BOMB_POWER);
}

ABomb::~ABomb()
{
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();
}

void ABomb::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	Running(_deltaTime);
}

/* Initialize */
void ABomb::InitSpriteAndAnim(const SBombTailTypes& _tailInfo)
{
	if (BombType == EBombType::PLAIN)
	{
		InitSpriteCenter(GlobalPath::BOMB_ORG, ANIM_RUNNING, IMG_EXPLOSION_CENTER);
	}
	else  // EBombType::RED
	{
		InitSpriteCenter(GlobalPath::BOMB_RED, ANIM_RUNNING, IMG_EXPLOSION_CENTER);
	}

	/* Create explosion animations */
	CreateTail(IMG_EXPLOSION_UP, IMG_EXPLOSION_UPMID, ANIM_EXPLODE_UP, { 0, -static_cast<int>(Size.Y) }, _tailInfo.Up, &ExplodeSprites_Up);
	CreateTail(IMG_EXPLOSION_DOWN, IMG_EXPLOSION_DOWNMID, ANIM_EXPLODE_DOWN, { 0, static_cast<int>(Size.Y) }, _tailInfo.Down, &ExplodeSprites_Down);
	CreateTail(IMG_EXPLOSION_LEFT, IMG_EXPLOSION_LEFTMID, ANIM_EXPLODE_LEFT, { -static_cast<int>(Size.X), 0 }, _tailInfo.Left, &ExplodeSprites_Left);
	CreateTail(IMG_EXPLOSION_RIGHT, IMG_EXPLOSION_RIGHTMID, ANIM_EXPLODE_RIGHT, { static_cast<int>(Size.X), 0 }, _tailInfo.Right, &ExplodeSprites_Right);

	SetState(BombState::Running);
}

void ABomb::CreateTail(const char* _img, const char* _imgMid, const char* _animName, const FVector2D& loc, const std::vector<EBombTailType>& _tails, std::vector<USpriteRenderer*>* _pAnimVec)
{
	for (size_t i = 0, size = _tails.size(); i < size; ++i)
	{
		if (_pAnimVec == nullptr) continue;

		int idx = static_cast<int>(i) + 1;
		if (_tails[i] == EBombTailType::CONNECT)
		{
			_InitDefaultSprite4D(_imgMid, _animName, *_pAnimVec, loc * static_cast<float>(idx));
		}
		else if (_tails[i] == EBombTailType::END)
		{
			_InitDefaultSprite4D(_img, _animName, *_pAnimVec, loc * static_cast<float>(idx));
		}
	}
}

void ABomb::InitSpriteCenter(std::string_view _spriteName, std::string_view _runnigAnimName, std::string_view _explodeSpriteName)
{
	_InitDefaultSprite(&ExplodeSprite_Center, _explodeSpriteName, ANIM_EXPLODE_CENTER, { 0, 0 });
	ExplodeSprite_Center->CreateAnimation(_runnigAnimName, _spriteName, 0, 3, .4f);

	ExplodeSprite_Center->SetAnimationEvent(ANIM_EXPLODE_CENTER, 1, std::bind(&ABomb::OnExploding, this));
	ExplodeSprite_Center->SetAnimationEvent(ANIM_EXPLODE_CENTER, 19, std::bind(&ABomb::OnEndAnimation, this));
	ExplodeSprite_Center->ChangeAnimation(_runnigAnimName);
}

// 4 directions
void ABomb::_InitDefaultSprite4D(std::string_view _spriteName, std::string_view _animName, std::vector<USpriteRenderer*>& _vector, const FVector2D& _loc)
{
	USpriteRenderer* spriteRenderer = nullptr;

	_InitDefaultSprite(&spriteRenderer, _spriteName, _animName, _loc);
	spriteRenderer->SetActive(false);

	_vector.push_back(spriteRenderer);
}

void ABomb::_InitDefaultSprite(USpriteRenderer** _spriteRenderer, std::string_view _spriteName, std::string_view _animName, const FVector2D& _animLoc)
{
	static const int MAX_FRAME = 20;
	static const int MID_SECONDS = 13;
	static const float FIRST_ANIM_SECS = .15f;
	static const float SECOND_ANIM_SECS = .1f;

	FVector2D loc = _animLoc + Size.Half();

	*_spriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	(*_spriteRenderer)->SetSprite(_spriteName);

	(*_spriteRenderer)->SetComponentLocation(loc);
	(*_spriteRenderer)->SetComponentScale(Size);
	(*_spriteRenderer)->SetPivotType(PivotType::Center);
	(*_spriteRenderer)->SetOrder(ERenderOrder::BOMB);

	std::vector<int> idxs;
	std::vector<float> secs;
	for (int i = 0; i < MAX_FRAME; ++i)
	{
		idxs.push_back(i);
		if (i < MID_SECONDS)
		{
			secs.push_back(FIRST_ANIM_SECS);
		}
		else
		{
			secs.push_back(SECOND_ANIM_SECS);
		}
	}

	(*_spriteRenderer)->CreateAnimation(_animName, _spriteName, idxs, secs, false);
}

/* Actions */
void ABomb::Running(float _deltaTime)
{
	if (State != BombState::Running) return;

	AccumulatedSeconds += _deltaTime;
	if (AccumulatedSeconds >= EXPLODE_SECONDS)
	{
		AccumulatedSeconds = 0;
		Explode();
	}
}

void ABomb::Explode()
{
	if (State != BombState::Running) return;
	SetState(BombState::StartExploding);
	RunExplosionAnim();
}

void ABomb::ExplodeIntermediatly()
{
	if (State == BombState::Exploding) return;
	SetState(BombState::Exploding);
	RunExplosionAnim();
}

/* Helpers */
void ABomb::RunExplosionAnim()
{
	_RunAnimHelper(ExplodeSprite_Center, ANIM_EXPLODE_CENTER);
	_RunAnimHelper(ExplodeSprites_Up, ANIM_EXPLODE_UP);
	_RunAnimHelper(ExplodeSprites_Down, ANIM_EXPLODE_DOWN);
	_RunAnimHelper(ExplodeSprites_Left, ANIM_EXPLODE_LEFT);
	_RunAnimHelper(ExplodeSprites_Right, ANIM_EXPLODE_RIGHT);
}

void ABomb::_RunAnimHelper(USpriteRenderer* _centerSprite, std::string_view _animName, bool _isOn)
{
	_centerSprite->ChangeAnimation(_animName);
	_centerSprite->SetActive(_isOn);
}

void ABomb::_RunAnimHelper(std::vector<USpriteRenderer*>& _vec, std::string_view _animName, bool _isOn)
{
	for (int i = 0; i < _vec.size(); ++i)
	{
		_vec[i]->ChangeAnimation(_animName);
		_vec[i]->SetActive(_isOn);
	}
}

/** Animation slots **/
void ABomb::OnExploding()
{
	if (State != BombState::StartExploding) return;

	SetState(BombState::Exploding);
}

void ABomb::OnEndAnimation()
{
	if (State != BombState::Exploding) return;

	SetState(BombState::Over);

	_RunAnimHelper(ExplodeSprite_Center, ANIM_EXPLODE_CENTER, false);
	_RunAnimHelper(ExplodeSprites_Up, ANIM_EXPLODE_UP, false);
	_RunAnimHelper(ExplodeSprites_Down, ANIM_EXPLODE_DOWN, false);
	_RunAnimHelper(ExplodeSprites_Left, ANIM_EXPLODE_LEFT, false);
	_RunAnimHelper(ExplodeSprites_Right, ANIM_EXPLODE_RIGHT, false);
}