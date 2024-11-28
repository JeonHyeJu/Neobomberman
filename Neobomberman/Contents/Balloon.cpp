#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "Balloon.h"
#include "GameData.h"
#include "BaseMap.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/2DCollision.h>

ABalloon::ABalloon()
{
	SetName("Balloon");
	CanHit = true;

	Fsm.CreateState(EMonsterState::BLINKING, std::bind(&ABalloon::Blinking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::INIT_WALKING, std::bind(&ABalloon::WalkingForStart, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::THINKING, std::bind(&ABalloon::Thinking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::WALKING, std::bind(&ABalloon::Walking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::DAMAGED, std::bind(&ABalloon::Damaging, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::DYING, std::bind(&ABalloon::Dying, this, std::placeholders::_1), std::bind(&ABalloon::OnDead, this));
	Fsm.CreateState(EMonsterState::PASS_AWAY, std::bind(&ABalloon::PassAwaing, this, std::placeholders::_1), std::bind(&ABalloon::OnPassaway, this));
}

ABalloon::~ABalloon()
{
}

void ABalloon::BeginPlay()
{
	AMonster::BeginPlay();

	InitSprite();
	InitCollision();
}

void ABalloon::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);

	if (!IsInited)
	{
		static float accSecs = 0.f;
		accSecs += _deltaTime;
		if (accSecs >= StartDelayMs)
		{
			IsInited = true;
			Fsm.ChangeState(EMonsterState::BLINKING);
			SRBody->SetActive(true);
		}

		return;	// TODO
	}

	Fsm.Update(_deltaTime);
}

void ABalloon::InitSprite()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;

	SRBody = CreateDefaultSubObject<USpriteRenderer>();
	SRBody->SetSprite(SPRITE_NAME);
	SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SRBody->SetComponentScale(size);
	SRBody->SetPivotType(PivotType::Bot);
	SRBody->SetOrder(ERenderOrder::MONSTER);

	SRBody->CreateAnimation(ANIM_RUN_UP, SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation(ANIM_RUN_DOWN, SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation(ANIM_RUN_LEFT, SPRITE_NAME, 0, 8, 0.5f);
	SRBody->CreateAnimation(ANIM_RUN_RIGHT, SPRITE_NAME, 0, 8, 0.5f);

	SetScore(EMonsterScore::S200);

	{
		FVector2D size = GlobalVar::BOMBERMAN_SIZE;
		SRCloud = CreateDefaultSubObject<USpriteRenderer>();
		SRCloud->SetSprite(CLOUD_SPRITE_PATH);
		SRCloud->SetComponentLocation(size.Half().Half());
		SRCloud->SetComponentScale(size);
		SRCloud->CreateAnimation(ANIM_CLOUD, CLOUD_SPRITE_PATH, 0, 7, .2f, false);
		SRCloud->SetOrder(ERenderOrder::MONSTER_CLOUD);
		SRCloud->SetActive(false);
	}

	{
		FVector2D size = GlobalVar::BOMB_SIZE;
		SRScore = CreateDefaultSubObject<USpriteRenderer>();
		SRScore->SetComponentLocation(size.Half().Half() + FVector2D{ size.Half().Half().X + 1.f, 0.f});
		SRScore->SetComponentScale(size);
		SRScore->SetOrder(ERenderOrder::MONSTER_SCORE);

		std::vector<int> idxs;
		std::vector<float> times(SCORE_ANIM_CNT, .2f);
		idxs.reserve(SCORE_ANIM_CNT);
		times.resize(SCORE_ANIM_CNT);
		for (int i = 0; i < SCORE_ANIM_CNT; ++i)
		{
			idxs.push_back(7+i);
		}
		times[SCORE_ANIM_CNT - 2] = 1.f;

		SRScore->CreateAnimation(ANIM_SCORE, MONSTER_SCORE_PATH, idxs, times, false);
		SRScore->SetActive(false);
	}

	SRBody->SetActive(false);
}

void ABalloon::InitCollision()
{
	FVector2D collSize = GlobalVar::BOMB_SIZE;
	Collision = CreateDefaultSubObject<U2DCollision>();
	Collision->SetComponentLocation({ collSize.hX(), 0.f });
	Collision->SetComponentScale(collSize);
	Collision->SetCollisionGroup(ECollisionGroup::MonsterBody);
	Collision->SetCollisionType(ECollisionType::CirCle);
}

FVector2D ABalloon::GetMonsterSize()
{
	return SRBody->GetComponentScale();
}

FIntPoint ABalloon::GetDamageRange()
{
	return FIntPoint{ 1, 1 };
}

void ABalloon::ChangeMoveAnim(const FVector2D& _direction)
{
	if (_direction == FVector2D::UP)
	{
		SRBody->ChangeAnimation(ANIM_RUN_UP);
	}
	else if (_direction == FVector2D::DOWN)
	{
		SRBody->ChangeAnimation(ANIM_RUN_DOWN);
	}
	else if (_direction == FVector2D::LEFT)
	{
		SRBody->ChangeAnimation(ANIM_RUN_LEFT);
	}
	else if (_direction == FVector2D::RIGHT)
	{
		SRBody->ChangeAnimation(ANIM_RUN_RIGHT);
	}
}

void ABalloon::Move(const FVector2D& _direction, float _deltaTime)
{
	bool isMove = false;

	if (CurMapPtr != nullptr)
	{
		FVector2D curLoc = GetActorLocation();
		FVector2D nextPosLT = curLoc + (_direction * _deltaTime * Speed) + FVector2D{ 1, 1 };
		FVector2D nextPosRT = curLoc + (_direction * _deltaTime * Speed) + FVector2D{ 31, 1 };
		FVector2D nextPosLB = curLoc + (_direction * _deltaTime * Speed) + FVector2D{ 1, 31 };
		FVector2D nextPosRB = curLoc + (_direction * _deltaTime * Speed) + FVector2D{ 31, 31 };

		bool canMoveMapLT = CurMapPtr->CanMove(nextPosLT);
		bool canMoveMapRT = CurMapPtr->CanMove(nextPosRT);
		bool canMoveMapLB = CurMapPtr->CanMove(nextPosLB);
		bool canMoveMapRB = CurMapPtr->CanMove(nextPosRB);
		isMove = canMoveMapLB && canMoveMapRB && canMoveMapLT && canMoveMapRT;
	}

	if (isMove)
	{
		ChangeMoveAnim(_direction);
		AddActorLocation(_direction * _deltaTime * Speed);
	}
	else
	{
		if (PrevIdx != FIntPoint::NEGATIVE_ONE)
		{
			ClearRoute();
			Route.push_back(PrevIdx);
		}
		
		Fsm.ChangeState(EMonsterState::THINKING);
	}
}

void ABalloon::Damaged(unsigned __int8 _power)
{
	if (static_cast<EMonsterState>(Fsm.GetState()) != EMonsterState::DAMAGED)
	{
		Health -= _power;
		Fsm.ChangeState(EMonsterState::DAMAGED);
	}
}

void ABalloon::Kill()
{
	if (Fsm.GetState() < static_cast<int>(EMonsterState::DYING))
	{
		if (Collision != nullptr)
		{
			Collision->SetActive(false);
		}
		Fsm.ChangeState(EMonsterState::DYING);
	}
}

void ABalloon::OnPause()
{
	if (SRBody)
	{
		SRBody->PauseCurAnimation();
	}
}

void ABalloon::OnResume()
{
	if (SRBody)
	{
		SRBody->ResumeCurAnimation();
	}
}

/* FSM start callbacks */
void ABalloon::OnDead()
{
	CanHit = false;
}

void ABalloon::OnPassaway()
{
	SRCloud->ChangeAnimation(ANIM_CLOUD, true);
	SRCloud->SetActive(true);

	SRScore->ChangeAnimation(ANIM_SCORE);
	SRScore->SetActive(true);

	GameData::GetInstance().AddPlayer1Score(GetScore());
}

/* FSM callbacks */
void ABalloon::Blinking(float _deltaTime)
{
	static float accumulatedSecs = 0.f;
	static float blinkElapsedSecs = 0.f;

	accumulatedSecs += _deltaTime;
	if (accumulatedSecs >= BLINK_SECONDS)
	{
		accumulatedSecs = 0.f;
		SRBody->SetActive(true);
		Fsm.ChangeState(EMonsterState::INIT_WALKING);
		return;
	}

	blinkElapsedSecs += _deltaTime;
	if (blinkElapsedSecs > 0.1f)
	{
		blinkElapsedSecs = 0.f;
		SRBody->SetActiveSwitch();
	}
}

void ABalloon::WalkingForStart(float _deltaTime)
{
	FVector2D firstLoc = CurMapPtr->MatrixIdxToLocation(FirstIdx);
	FVector2D monsterRealLoc = GetActorLocation();

	FIntPoint firstLocInt = firstLoc.ConvertToPoint();
	FIntPoint monsterRealLocInt = monsterRealLoc.ConvertToPoint();

	if (monsterRealLocInt.X > firstLocInt.X)
	{
		Move(FVector2D::LEFT, _deltaTime);
	}
	else if (monsterRealLocInt.Y > firstLocInt.Y)
	{
		Move(FVector2D::UP, _deltaTime);
	}
	else if (monsterRealLocInt.X < firstLocInt.X)
	{
		Move(FVector2D::RIGHT, _deltaTime);
	}
	else
	{
		Fsm.ChangeState(EMonsterState::THINKING);
	}
}

void ABalloon::Thinking(float _deltaTime)
{
	if (IsRouteEmpty())
	{
		FindPath();
	}
	else
	{
		Fsm.ChangeState(EMonsterState::WALKING);
	}
}

void ABalloon::Walking(float _deltaTime)
{
	if (IsRouteEmpty())
	{
		Fsm.ChangeState(EMonsterState::THINKING);
		return;
	}

	if (IsArrivedAtOneBlock())
	{
		PrevIdx = Destination;
		Destination = Route.front();
		Route.pop_front();
	}

	FVector2D monsterRealLoc = GetActorLocation();
	FIntPoint monsterRealLocInt = monsterRealLoc.ConvertToPoint();
	FIntPoint monsterMatrixIdx = CurMapPtr->LocationToMatrixIdx(monsterRealLoc);

	FVector2D destRealLoc = CurMapPtr->MatrixIdxToLocation(Destination);
	FIntPoint destRealLocInt = destRealLoc.ConvertToPoint();
	FIntPoint destMatrixIdx = Destination;

	//OutputDebugString(("monsterRealLoc: " + std::to_string(monsterRealLoc.X) + ", " + std::to_string(monsterRealLoc.Y) + " ... " + ("destRealLoc: " + std::to_string(destRealLoc.X) + ", " + std::to_string(destRealLoc.Y) + "\n")).c_str());
	//OutputDebugString(("monsterMatrixIdx: " + std::to_string(monsterMatrixIdx.X) + ", " + std::to_string(monsterMatrixIdx.Y) + " ... " + ("destMatrixIdx: " + std::to_string(destMatrixIdx.X) + ", " + std::to_string(destMatrixIdx.Y) + "\n")).c_str());

	if (monsterMatrixIdx == destMatrixIdx)
	{
		//OutputDebugString("monsterIdx == Destination\n");
		bool isGotLast = (monsterRealLocInt.X == destRealLocInt.X) && (monsterRealLocInt.Y <= destRealLocInt.Y);
		if (isGotLast)
		{
			//OutputDebugString("isGotLast\n");
			Destination = FIntPoint::NEGATIVE_ONE;
			return;
		}
	}

	FVector2D direction = GetDirection(destRealLocInt - monsterRealLocInt);

	Move(direction, _deltaTime);
}

void ABalloon::Damaging(float _deltaTime)
{
	// TODO: blinking..

	if (Health <= 0)
	{
		Kill();
	}
}

void ABalloon::Dying(float _deltaTime)
{
	static float accumulatedSecs = 0.f;
	static float blinkElapsedSecs = 0.f;

	accumulatedSecs += _deltaTime;
	if (accumulatedSecs >= BLINK_SECONDS)
	{
		accumulatedSecs = 0.f;
		SRBody->SetActive(false);
		Fsm.ChangeState(EMonsterState::PASS_AWAY);
		return;
	}

	blinkElapsedSecs += _deltaTime;
	if (blinkElapsedSecs > 0.1f)
	{
		blinkElapsedSecs = 0.f;
		SRBody->SetActiveSwitch();
	}
}

void ABalloon::PassAwaing(float _deltaTime)
{
	if (IsDestroiable) return;

	if (SRCloud->IsCurAnimationEnd() && SRScore->IsCurAnimationEnd())
	{
		IsDestroiable = true;
	}
}