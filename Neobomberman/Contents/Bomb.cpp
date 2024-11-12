#include "PreCompile.h"
#include "Bomb.h"
#include "TileMap.h"
#include "PlayMap.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "DebugLog.h"	// TODO: delete

#include <EngineCore/SpriteRenderer.h>
#include <EngineBase/EngineDebug.h>
#include <functional>
#include <algorithm>

std::list<ABomb*> ABomb::BombList;

void _ReorginizeExplosion(std::vector<EBombTailType>& _bombTrails)
{
	for (size_t i = 0, size = _bombTrails.size(); i < size - 1; ++i)
	{
		if (i + 1 >= size) break;

		if (_bombTrails[i] == EBombTailType::END && _bombTrails[i + 1] == EBombTailType::END)
		{
			_bombTrails[i] = EBombTailType::CONNECT;
		}
		else if (_bombTrails[i] == EBombTailType::END && _bombTrails[i + 1] == EBombTailType::CONNECT)
		{
			_bombTrails[i] = EBombTailType::CONNECT;
		}
	}
}

ABomb::ABomb()
{
	ExplodeSprites_Up.reserve(GlobalVar::MAX_BOMB_POWER);
	ExplodeSprites_Down.reserve(GlobalVar::MAX_BOMB_POWER);
	ExplodeSprites_Left.reserve(GlobalVar::MAX_BOMB_POWER);
	ExplodeSprites_Right.reserve(GlobalVar::MAX_BOMB_POWER);

	FSM.CreateState(EBombState::Running, std::bind(&ABomb::Running, this, std::placeholders::_1), nullptr);
	FSM.CreateState(EBombState::Launched, nullptr, std::bind(&ABomb::OnLaunched, this));
	FSM.CreateState(EBombState::Exploding, nullptr, nullptr);
	FSM.CreateState(EBombState::Over, nullptr, nullptr);
}

ABomb::~ABomb()
{
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();

	FSM.ChangeState(EBombState::Running);
}

void ABomb::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	FSM.UpdateState(_deltaTime);
}

/* Initialize */
void ABomb::Init(const FVector2D& _loc, EBombType _bombType, int _power, APlayMap* _curMap)
{
	if (ABomb::BombList.size() >= GlobalVar::MAX_BOMB_CNT) return;

	SetCurMap(_curMap);
	ATileMap* pMap = CurMap->GetGroundMap();
	FIntPoint matIdx = pMap->LocationToMatrixIdx(_loc);

	if (!CanSetBombThisIdx(matIdx)) return;

	FIntPoint realIdx = pMap->LocationToIndex(_loc);
	FVector2D orderedLoc = pMap->IndexToLocation(realIdx);
	SBombTailTypes bombTailTypes = GetBombTailTypes(matIdx, _bombType, _power);
	std::vector<FIntPoint> explodeIdxs = GetBombRange(matIdx, bombTailTypes);

	//DebugPrintEnumVector(bombTailTypes.Up, "bombTailTypes[UP]");
	//DebugPrintEnumVector(bombTailTypes.Down, "bombTailTypes[Down]");
	//DebugPrintEnumVector(bombTailTypes.Left, "bombTailTypes[Left]");
	//DebugPrintEnumVector(bombTailTypes.Right, "bombTailTypes[Right]");
	//DebugPrintFIntVector(explodeIdxs, "explodeIdxs");

	Size = GlobalVar::BOMB_SIZE;
	Power = _power;
	BombType = _bombType;
	MatrixIdx = matIdx;
	ExplodeIdxs = explodeIdxs;

	SetActorLocation(orderedLoc);
	InitSpriteAndAnim(bombTailTypes);

	ABomb::BombList.push_back(this);
}

void ABomb::InitSpriteAndAnim(const SBombTailTypes& _tailInfo)
{
	if (BombType == EBombType::PLAIN)
	{
		InitSpriteCenter(GlobalPath::BOMB_ORG, ANIM_BOMB_RUNNING, IMG_EXPLOSION_CENTER);
	}
	else  // EBombType::RED
	{
		InitSpriteCenter(GlobalPath::BOMB_RED, ANIM_BOMB_RUNNING, IMG_EXPLOSION_CENTER);
	}

	/* Create explosion animations */
	CreateTail(IMG_EXPLOSION_UP, IMG_EXPLOSION_UPMID, ANIM_EXPLODE_UP, { 0, -static_cast<int>(Size.Y) }, _tailInfo.Up, &ExplodeSprites_Up);
	CreateTail(IMG_EXPLOSION_DOWN, IMG_EXPLOSION_DOWNMID, ANIM_EXPLODE_DOWN, { 0, static_cast<int>(Size.Y) }, _tailInfo.Down, &ExplodeSprites_Down);
	CreateTail(IMG_EXPLOSION_LEFT, IMG_EXPLOSION_LEFTMID, ANIM_EXPLODE_LEFT, { -static_cast<int>(Size.X), 0 }, _tailInfo.Left, &ExplodeSprites_Left);
	CreateTail(IMG_EXPLOSION_RIGHT, IMG_EXPLOSION_RIGHTMID, ANIM_EXPLODE_RIGHT, { static_cast<int>(Size.X), 0 }, _tailInfo.Right, &ExplodeSprites_Right);
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
	// Creating a center explosion animation first
	_InitDefaultSprite(&ExplodeSprite_Center, _explodeSpriteName, ANIM_EXPLODE_CENTER, { 0, 0 });
	
	// Craeteing running bomb animation
	ExplodeSprite_Center->CreateAnimation(_runnigAnimName, _spriteName, 0, 3, .4f);

	ExplodeSprite_Center->SetAnimationEvent(ANIM_EXPLODE_CENTER, 1, std::bind(&ABomb::OnStartAnimation, this));
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

SBombTailTypes ABomb::GetBombTailTypes(const FIntPoint& _matIdx, EBombType _bombType, int _power)
{
	SBombTailTypes bombTailTypes;

	bool isUpEnd = false;
	bool isDownEnd = false;
	bool isLeftEnd = false;
	bool isRightEnd = false;

	ATileMap* pMapWall = CurMap->GetWallMap();
	ATileMap* pBoxWall = CurMap->GetBoxMap();

	for (int i = 1; i <= _power; i++)
	{
		bool isLast = (i == _power);
		EBombTailType upType = GetBombTailType(pMapWall, pBoxWall, _matIdx + FIntPoint::UP * i, &isUpEnd, isLast);
		EBombTailType downType = GetBombTailType(pMapWall, pBoxWall, _matIdx + FIntPoint::DOWN * i, &isDownEnd, isLast);
		EBombTailType leftType = GetBombTailType(pMapWall, pBoxWall, _matIdx + FIntPoint::LEFT * i, &isLeftEnd, isLast);
		EBombTailType rightType = GetBombTailType(pMapWall, pBoxWall, _matIdx + FIntPoint::RIGHT * i, &isRightEnd, isLast);

		if (upType != EBombTailType::NONE) bombTailTypes.Up.push_back(upType);
		if (downType != EBombTailType::NONE) bombTailTypes.Down.push_back(downType);
		if (leftType != EBombTailType::NONE) bombTailTypes.Left.push_back(leftType);
		if (rightType != EBombTailType::NONE) bombTailTypes.Right.push_back(rightType);
	}

	_ReorginizeExplosion(bombTailTypes.Up);
	_ReorginizeExplosion(bombTailTypes.Down);
	_ReorginizeExplosion(bombTailTypes.Left);
	_ReorginizeExplosion(bombTailTypes.Right);

	return bombTailTypes;
}

EBombTailType ABomb::GetBombTailType(ATileMap* _pWallMap, ATileMap* _pBoxMap, const FIntPoint& _nextIdx, bool* _isEnd, bool _isLast)
{
	if (CurMap == nullptr)
	{
		MSGASSERT("맵이 세팅되지 않은채 폭탄을 설치했습니다.");
	}

	bool hasWall = _pWallMap->IsBlocked(_nextIdx);
	bool hasBox = _pBoxMap->IsBlocked(_nextIdx);

	if (hasWall == false && *_isEnd == false)
	{
		if (hasBox == true)
		{
			*_isEnd = true;
			return EBombTailType::CONNECT;
		}
		else
		{
			if (_isLast)
			{
				return EBombTailType::END;
			}
			else if (_pWallMap->IsEdge(_nextIdx))
			{
				return EBombTailType::CONNECT;
			}
			else
			{
				return EBombTailType::CONNECT;
			}
		}
	}
	else
	{
		*_isEnd = true;
	}

	return EBombTailType::NONE;
}

std::vector<FIntPoint> ABomb::GetBombRange(const FIntPoint& _matIdx, const SBombTailTypes& _tailInfo)
{
	std::vector<FIntPoint> explodeIdxs;
	explodeIdxs.reserve(GlobalVar::MAX_BOMB_POWER * 4);
	explodeIdxs.push_back(_matIdx);

	for (size_t i = 0, size = _tailInfo.Up.size(); i < size; ++i)
	{
		explodeIdxs.push_back(_matIdx + FIntPoint::UP * (static_cast<int>(i) + 1));
	}
	for (size_t i = 0, size = _tailInfo.Down.size(); i < size; ++i)
	{
		explodeIdxs.push_back(_matIdx + FIntPoint::DOWN * (static_cast<int>(i) + 1));
	}
	for (size_t i = 0, size = _tailInfo.Left.size(); i < size; ++i)
	{
		explodeIdxs.push_back(_matIdx + FIntPoint::LEFT * (static_cast<int>(i) + 1));
	}
	for (size_t i = 0, size = _tailInfo.Right.size(); i < size; ++i)
	{
		explodeIdxs.push_back(_matIdx + FIntPoint::RIGHT * (static_cast<int>(i) + 1));
	}

	return explodeIdxs;
}

/* FSM actions */
void ABomb::ExplodeBySplash()
{
	OutputDebugStringA("ABomb::ExplodeBySplash()\n");
	FSM.ChangeState(EBombState::Launched);
}

void ABomb::Running(float _deltaTime)
{
	if (GetState() != EBombState::Running) return;

	AccumulatedSeconds += _deltaTime;
	if (AccumulatedSeconds >= EXPLODE_SECONDS)
	{
		AccumulatedSeconds = 0;
		FSM.ChangeState(EBombState::Launched);
	}
}

void ABomb::OnLaunched()
{
	RunExplosionAnim(true);
}

/** Animation slots **/
void ABomb::OnStartAnimation()
{
	FSM.ChangeState(EBombState::Exploding);
}

void ABomb::OnEndAnimation()
{
	RunExplosionAnim(false);
	FSM.ChangeState(EBombState::Over);
}

/* Helpers */
void ABomb::RunExplosionAnim(bool _isOn)
{
	_RunAnimHelper(ExplodeSprite_Center, ANIM_EXPLODE_CENTER, _isOn);
	_RunAnimHelper(ExplodeSprites_Up, ANIM_EXPLODE_UP, _isOn);
	_RunAnimHelper(ExplodeSprites_Down, ANIM_EXPLODE_DOWN, _isOn);
	_RunAnimHelper(ExplodeSprites_Left, ANIM_EXPLODE_LEFT, _isOn);
	_RunAnimHelper(ExplodeSprites_Right, ANIM_EXPLODE_RIGHT, _isOn);
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