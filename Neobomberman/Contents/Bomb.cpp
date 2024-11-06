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

void ABomb::InitOrgBomb(int _power)
{
	InitSprite(RESOURCE_PLAINBOMB_PATH, ANIM_RUNNING, TEMP_EXPLODE_SPRITE_NAME, ANIM_EXPLODE);	// TODO
	InitExplodeSprites(_power);
}

void ABomb::InitRedBomb(int _power)
{
	InitSprite(RESOURCE_REDBOMB_PATH, ANIM_RUNNING, TEMP_EXPLODE_SPRITE_NAME, ANIM_EXPLODE);	// TODO
	InitExplodeSprites(_power);
}

void ABomb::InitSprite(std::string_view _spriteName, std::string_view _runnigAnimName, std::string_view _explodeSpriteName, std::string_view _explodeAnimName)
{
	//FVector2D size = GlobalVar::BOMB_SIZE;
	//InitExplodeSprite(ExplodeSprites_Center, _explodeSpriteName, _explodeAnimName, { 0, 0 });

	FVector2D size = GlobalVar::BOMB_SIZE;	// Temp
	FVector2D sizeHalf = size.Half();

	ExplodeSprites_Center = CreateDefaultSubObject<USpriteRenderer>();
	ExplodeSprites_Center->SetSprite(_spriteName);

	ExplodeSprites_Center->SetComponentLocation(sizeHalf);
	ExplodeSprites_Center->SetComponentScale(size);
	ExplodeSprites_Center->SetPivotType(PivotType::Center);

	ExplodeSprites_Center->CreateAnimation(_runnigAnimName, _spriteName, 0, 3, .4f);
	ExplodeSprites_Center->CreateAnimation(_explodeAnimName, _explodeSpriteName, 0, 19, .15f);

	//ExplodeSprites_Center->SetActive(false);
	
	ExplodeAnimName = _explodeAnimName;

	// Temp
	ExplodeSprites_Center->SetAnimationEvent(_explodeAnimName, 19, std::bind(&ABomb::OnEndAnimation, this));
	ExplodeSprites_Center->SetOrder(ERenderOrder::BOMB);

	ExplodeSprites_Center->ChangeAnimation(_runnigAnimName);

	State = BombState::Running;
}

void ABomb::InitExplodeSprite(USpriteRenderer* _spriteRenderer, std::string_view _spriteName, std::string_view _animName, const FVector2D& _loc)
{
	FVector2D size = GlobalVar::BOMB_SIZE;	// Temp
	FVector2D sizeHalf = size.Half();

	_spriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	_spriteRenderer->SetSprite(_spriteName);

	_spriteRenderer->SetComponentLocation(_loc + sizeHalf);
	_spriteRenderer->SetComponentScale(size);
	_spriteRenderer->SetPivotType(PivotType::Center);

	_spriteRenderer->CreateAnimation(_animName, _spriteName, 0, 19, .15f);
	_spriteRenderer->SetActive(false);
}

void ABomb::InitExplodeSpriteVector(std::string_view _spriteName, std::string_view _animName, std::vector<USpriteRenderer*>& _vector, const FVector2D& _loc)
{
	USpriteRenderer* spriteRenderer = nullptr;
	InitExplodeSprite(spriteRenderer, _spriteName, _animName, _loc);
	_vector.push_back(spriteRenderer);
}

void ABomb::InitExplodeSprites(int _power)
{
	Power = _power;
	FVector2D size = GlobalVar::BOMB_SIZE;	// Temp

	for (int i = 0; i < _power; i++)
	{
		int idx = (i + 1);
		if (i == 0 || i == (_power - 1))
		{
			InitExplodeSpriteVector("ExplodeUp.png", "ExplodeUp", ExplodeSprites_Up, { 0, -static_cast<int>(idx * size.Y) });
			InitExplodeSpriteVector("ExplodeDown.png", "ExplodeDown", ExplodeSprites_Down, { 0, static_cast<int>(idx * size.Y) });
			InitExplodeSpriteVector("ExplodeLeft.png", "ExplodeLeft", ExplodeSprites_Left, { -static_cast<int>(idx * size.X), 0 });
			InitExplodeSpriteVector("ExplodeRight.png", "ExplodeRight", ExplodeSprites_Right, { static_cast<int>(idx * size.X), 0 });
		}
		else
		{
			InitExplodeSpriteVector("ExplodeUpMid.png", "ExplodeUpMid", ExplodeSprites_Up, { 0, static_cast<int>(idx * size.Y) });
			InitExplodeSpriteVector("ExplodeDownMid.png", "ExplodeDownMid", ExplodeSprites_Down, { 0, -static_cast<int>(idx * size.Y) });
			InitExplodeSpriteVector("ExplodeLeftMid.png", "ExplodeLeftMid", ExplodeSprites_Left, { -static_cast<int>(idx * size.X), 0 });
			InitExplodeSpriteVector("ExplodeRightMid.png", "ExplodeRightMid", ExplodeSprites_Right, { static_cast<int>(idx * size.X), 0 });
		}
	}
}

void ABomb::RunAnimHelper(std::vector<USpriteRenderer*>& _vec, std::string_view _animName, bool _isOn)
{
	for (int i = 0; i < _vec.size(); ++i)
	{
		_vec[i]->ChangeAnimation(_animName);
		_vec[i]->SetActive(_isOn);
	}
}

void ABomb::Explode()
{
	if (State == BombState::Running)
	{
		State = BombState::Exploding;
		UEngineDebug::OutPutString("BOOM!!!!!!!!!!!!!!!!!!!!!!!!!!");
		ExplodeSprites_Center->ChangeAnimation(ExplodeAnimName);
		
		RunAnimHelper(ExplodeSprites_Up, "ExplodeUp");
		RunAnimHelper(ExplodeSprites_Down, "ExplodeDown");
		RunAnimHelper(ExplodeSprites_Left, "ExplodeLeft");
		RunAnimHelper(ExplodeSprites_Right, "ExplodeRight");
	}
}

void ABomb::OnEndAnimation()
{
	if (State == BombState::Exploding)
	{
		State = BombState::Over;
		UEngineDebug::OutPutString("Over~~~~~~~~~~~~~~~~~~~");

		bool isOff = false;

		// Off animation
		ExplodeSprites_Center->ChangeAnimation(ExplodeAnimName);
		ExplodeSprites_Center->SetActive(isOff);

		RunAnimHelper(ExplodeSprites_Up, "ExplodeUp", isOff);
		RunAnimHelper(ExplodeSprites_Down, "ExplodeDown", isOff);
		RunAnimHelper(ExplodeSprites_Left, "ExplodeLeft", isOff);
		RunAnimHelper(ExplodeSprites_Right, "ExplodeRight", isOff);

		// TODO: BombManager delete ptr vector
	}
}