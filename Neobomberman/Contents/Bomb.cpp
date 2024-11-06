#include "PreCompile.h"
#include "Bomb.h"
#include "Timer.h"

#ifdef _DEBUG
#include <iostream>
#endif

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

	ExplodeTileLocs.reserve(GlobalVar::MAX_BOMB_POWER * 4);
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
			Explode();
		}
	}
}

void ABomb::InitOrgBomb(const FVector2D& _loc, int _power)
{
	SetActorLocation(_loc);
	InitSpriteCenter(RESOURCE_PLAINBOMB_PATH, ANIM_RUNNING, TEMP_EXPLODE_SPRITE_NAME);	// TODO
	InitExplodeSprites(_power);
}

void ABomb::InitRedBomb(const FVector2D& _loc, int _power)
{
	SetActorLocation(_loc);
	InitSpriteCenter(RESOURCE_REDBOMB_PATH, ANIM_RUNNING, TEMP_EXPLODE_SPRITE_NAME);	// TODO
	InitExplodeSprites(_power);
}

void ABomb::InitSpriteCenter(std::string_view _spriteName, std::string_view _runnigAnimName, std::string_view _explodeSpriteName)
{
	InitExplodeSprite(&ExplodeSprites_Center, _explodeSpriteName, "ExplodeCenter", {0, 0});
	ExplodeSprites_Center->CreateAnimation(_runnigAnimName, _spriteName, 0, 3, .4f);

	// Temp
	ExplodeSprites_Center->SetAnimationEvent("ExplodeCenter", 19, std::bind(&ABomb::OnEndAnimation, this));
	ExplodeSprites_Center->ChangeAnimation(_runnigAnimName);

	State = BombState::Running;
}

void ABomb::InitExplodeSprite(USpriteRenderer** _spriteRenderer, std::string_view _spriteName, std::string_view _animName, const FVector2D& _loc)
{
	FVector2D size = GlobalVar::BOMB_SIZE;	// Temp
	FVector2D sizeHalf = size.Half();
	FVector2D loc = _loc + sizeHalf;

	*_spriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	(*_spriteRenderer)->SetSprite(_spriteName);

	(*_spriteRenderer)->SetComponentLocation(loc);
	(*_spriteRenderer)->SetComponentScale(size);
	(*_spriteRenderer)->SetPivotType(PivotType::Center);
	(*_spriteRenderer)->SetOrder(ERenderOrder::BOMB);

	std::vector<int> idxs;
	std::vector<float> secs;
	for (int i = 0; i < 20; ++i)
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

	ExplodeTileLocs.push_back(GetActorLocation() + loc);
}

void ABomb::InitExplodeSpriteVector(std::string_view _spriteName, std::string_view _animName, std::vector<USpriteRenderer*>& _vector, const FVector2D& _loc)
{
	USpriteRenderer* spriteRenderer = nullptr;

	InitExplodeSprite(&spriteRenderer, _spriteName, _animName, _loc);
	spriteRenderer->SetActive(false);

	_vector.push_back(spriteRenderer);
}

void ABomb::InitExplodeSprites(int _power)
{
	Power = _power;
	FVector2D size = GlobalVar::BOMB_SIZE;	// Temp

	// TODO: orginize
	if (_power == 1)
	{
		InitExplodeSpriteVector("ExplodeUp.png", "ExplodeUp", ExplodeSprites_Up, { 0, -static_cast<int>(size.Y) });
		InitExplodeSpriteVector("ExplodeDown.png", "ExplodeDown", ExplodeSprites_Down, { 0, static_cast<int>(size.Y) });
		InitExplodeSpriteVector("ExplodeLeft.png", "ExplodeLeft", ExplodeSprites_Left, { -static_cast<int>(size.X), 0 });
		InitExplodeSpriteVector("ExplodeRight.png", "ExplodeRight", ExplodeSprites_Right, { static_cast<int>(size.X), 0 });
	}
	else
	{
		for (int i = 0; i < _power; i++)
		{
			int idx = i + 1;
			if (i < (_power - 1))
			{
				InitExplodeSpriteVector("ExplodeUpMid.png", "ExplodeUp", ExplodeSprites_Up, { 0, -static_cast<int>(idx * size.Y) });
				InitExplodeSpriteVector("ExplodeDownMid.png", "ExplodeDown", ExplodeSprites_Down, { 0, static_cast<int>(idx * size.Y) });
				InitExplodeSpriteVector("ExplodeLeftMid.png", "ExplodeLeft", ExplodeSprites_Left, { -static_cast<int>(idx * size.X), 0 });
				InitExplodeSpriteVector("ExplodeRightMid.png", "ExplodeRight", ExplodeSprites_Right, { static_cast<int>(idx * size.X), 0 });
			}
			else
			{
				InitExplodeSpriteVector("ExplodeUp.png", "ExplodeUp", ExplodeSprites_Up, { 0, -static_cast<int>(idx * size.Y) });
				InitExplodeSpriteVector("ExplodeDown.png", "ExplodeDown", ExplodeSprites_Down, { 0, static_cast<int>(idx * size.Y) });
				InitExplodeSpriteVector("ExplodeLeft.png", "ExplodeLeft", ExplodeSprites_Left, { -static_cast<int>(idx * size.X), 0 });
				InitExplodeSpriteVector("ExplodeRight.png", "ExplodeRight", ExplodeSprites_Right, { static_cast<int>(idx * size.X), 0 });
			}
		}
	}
}

void ABomb::Explode()
{
	if (State == BombState::Running)
	{
		State = BombState::Exploding;
		ExplodeSprites_Center->ChangeAnimation("ExplodeCenter");
		
		_RunAnimHelper(ExplodeSprites_Up, "ExplodeUp");
		_RunAnimHelper(ExplodeSprites_Down, "ExplodeDown");
		_RunAnimHelper(ExplodeSprites_Left, "ExplodeLeft");
		_RunAnimHelper(ExplodeSprites_Right, "ExplodeRight");
	}
}

void ABomb::OnEndAnimation()
{
	if (State == BombState::Exploding)
	{
		State = BombState::Over;

		bool isOff = false;

		// Off animation
		ExplodeSprites_Center->ChangeAnimation("ExplodeCenter");
		ExplodeSprites_Center->SetActive(isOff);

		_RunAnimHelper(ExplodeSprites_Up, "ExplodeUp", isOff);
		_RunAnimHelper(ExplodeSprites_Down, "ExplodeDown", isOff);
		_RunAnimHelper(ExplodeSprites_Left, "ExplodeLeft", isOff);
		_RunAnimHelper(ExplodeSprites_Right, "ExplodeRight", isOff);
	}
}

void ABomb::_RunAnimHelper(std::vector<USpriteRenderer*>& _vec, std::string_view _animName, bool _isOn)
{
	for (int i = 0; i < _vec.size(); ++i)
	{
		_vec[i]->ChangeAnimation(_animName);
		_vec[i]->SetActive(_isOn);
	}
}