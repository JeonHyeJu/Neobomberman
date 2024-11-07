
#include "PreCompile.h"
#include "Bomb.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"

#include <EngineCore/SpriteRenderer.h>
#include <EngineBase/EngineDebug.h>
#include <functional>

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

void ABomb::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (State == BombState::Running)
	{
		AccumulatedSeconds += _DeltaTime;
		if (AccumulatedSeconds >= EXPLODE_SECONDS)
		{
			AccumulatedSeconds = 0;
			Explode();
		}
	}
}

void ABomb::Create(const FVector2D& _loc, const FVector2D& _Size, int _power, EBombType _type)
{
	Size = _Size;
	Power = _power;
	BombType = _type;
	SetActorLocation(_loc - Size.Half());

	/* Initialize */
	if (_type == EBombType::PLAIN)
	{
		InitSpriteCenter(GlobalPath::BOMB_ORG, ANIM_RUNNING, IMG_EXPLOSION_CENTER);
	}
	else  // EBombType::RED
	{
		InitSpriteCenter(GlobalPath::BOMB_RED, ANIM_RUNNING, IMG_EXPLOSION_CENTER);
	}
	InitSprite4Direction();

	SetState(BombState::Running);
}

void ABomb::InitSpriteCenter(std::string_view _spriteName, std::string_view _runnigAnimName, std::string_view _explodeSpriteName)
{
	_InitDefaultSprite(&ExplodeSprite_Center, _explodeSpriteName, ANIM_EXPLODE_CENTER, { 0, 0 });
	ExplodeSprite_Center->CreateAnimation(_runnigAnimName, _spriteName, 0, 3, .4f);

	ExplodeSprite_Center->SetAnimationEvent(ANIM_EXPLODE_CENTER, 1, std::bind(&ABomb::OnExploding, this));
	ExplodeSprite_Center->SetAnimationEvent(ANIM_EXPLODE_CENTER, 19, std::bind(&ABomb::OnEndAnimation, this));
	ExplodeSprite_Center->ChangeAnimation(_runnigAnimName);
}

void ABomb::InitSprite4Direction()
{
	if (Power == GlobalVar::ORG_BOMB_POWER)
	{
		_InitDefaultSprite4D(IMG_EXPLOSION_UP, ANIM_EXPLODE_UP, ExplodeSprites_Up, { 0, -static_cast<int>(Size.Y) });
		_InitDefaultSprite4D(IMG_EXPLOSION_DOWN, ANIM_EXPLODE_DOWN, ExplodeSprites_Down, { 0, static_cast<int>(Size.Y) });
		_InitDefaultSprite4D(IMG_EXPLOSION_LEFT, ANIM_EXPLODE_LEFT, ExplodeSprites_Left, { -static_cast<int>(Size.X), 0 });
		_InitDefaultSprite4D(IMG_EXPLOSION_RIGHT, ANIM_EXPLODE_RIGHT, ExplodeSprites_Right, { static_cast<int>(Size.X), 0 });
	}
	else
	{
		for (int i = 0; i < Power; i++)
		{
			int idx = i + 1;
			if (i < (Power - 1))
			{
				_InitDefaultSprite4D(IMG_EXPLOSION_UPMID, ANIM_EXPLODE_UP, ExplodeSprites_Up, { 0, -static_cast<int>(idx * Size.Y) });
				_InitDefaultSprite4D(IMG_EXPLOSION_DOWNMID, ANIM_EXPLODE_DOWN, ExplodeSprites_Down, { 0, static_cast<int>(idx * Size.Y) });
				_InitDefaultSprite4D(IMG_EXPLOSION_LEFTMID, ANIM_EXPLODE_LEFT, ExplodeSprites_Left, { -static_cast<int>(idx * Size.X), 0 });
				_InitDefaultSprite4D(IMG_EXPLOSION_RIGHTMID, ANIM_EXPLODE_RIGHT, ExplodeSprites_Right, { static_cast<int>(idx * Size.X), 0 });
			}
			else
			{
				_InitDefaultSprite4D(IMG_EXPLOSION_UP, ANIM_EXPLODE_UP, ExplodeSprites_Up, { 0, -static_cast<int>(idx * Size.Y) });
				_InitDefaultSprite4D(IMG_EXPLOSION_DOWN, ANIM_EXPLODE_DOWN, ExplodeSprites_Down, { 0, static_cast<int>(idx * Size.Y) });
				_InitDefaultSprite4D(IMG_EXPLOSION_LEFT, ANIM_EXPLODE_LEFT, ExplodeSprites_Left, { -static_cast<int>(idx * Size.X), 0 });
				_InitDefaultSprite4D(IMG_EXPLOSION_RIGHT, ANIM_EXPLODE_RIGHT, ExplodeSprites_Right, { static_cast<int>(idx * Size.X), 0 });
			}
		}
	}
}

void ABomb::_InitDefaultSprite4D(std::string_view _spriteName, std::string_view _animName, std::vector<USpriteRenderer*>& _vector, const FVector2D& _loc)
{
	USpriteRenderer* spriteRenderer = nullptr;

	_InitDefaultSprite(&spriteRenderer, _spriteName, _animName, _loc);
	spriteRenderer->SetActive(false);

	_vector.push_back(spriteRenderer);
}

void ABomb::_InitDefaultSprite(USpriteRenderer** _spriteRenderer, std::string_view _spriteName, std::string_view _animName, const FVector2D& _animLoc)
{
	const int MAX_FRAME = 20;

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
		if (i < 13)
		{
			secs.push_back(.15f);
		}
		else
		{
			secs.push_back(.1f);
		}
	}

	(*_spriteRenderer)->CreateAnimation(_animName, _spriteName, idxs, secs, false);
}

void ABomb::Explode()
{
	if (State == BombState::Running)
	{
		SetState(BombState::StartExploding);

		_RunAnimHelper(ExplodeSprite_Center, ANIM_EXPLODE_CENTER);
		_RunAnimHelper(ExplodeSprites_Up, ANIM_EXPLODE_UP);
		_RunAnimHelper(ExplodeSprites_Down, ANIM_EXPLODE_DOWN);
		_RunAnimHelper(ExplodeSprites_Left, ANIM_EXPLODE_LEFT);
		_RunAnimHelper(ExplodeSprites_Right, ANIM_EXPLODE_RIGHT);
	}
}

/* Helpers */
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

/** slots **/
void ABomb::OnExploding()
{
	if (State == BombState::StartExploding)
	{
		SetState(BombState::Exploding);
	}
}

void ABomb::OnEndAnimation()
{
	if (State == BombState::FinishExploding)
	{
		SetState(BombState::Over);

		bool isOn = false;
		_RunAnimHelper(ExplodeSprite_Center, ANIM_EXPLODE_CENTER, isOn);
		_RunAnimHelper(ExplodeSprites_Up, ANIM_EXPLODE_UP, isOn);
		_RunAnimHelper(ExplodeSprites_Down, ANIM_EXPLODE_DOWN, isOn);
		_RunAnimHelper(ExplodeSprites_Left, ANIM_EXPLODE_LEFT, isOn);
		_RunAnimHelper(ExplodeSprites_Right, ANIM_EXPLODE_RIGHT, isOn);
	}
}