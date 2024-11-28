#include "PreCompile.h"
#include "Mushroom.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "GameData.h"
#include "BaseMap.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/PathFindAStar.h>
#include <EngineCore/2DCollision.h>
#include <EnginePlatform/EngineSound.h>

AMushroom::AMushroom()
{
	Random.SetSeed(MonsterIdx);
	CanHit = true;

	SetName("Mushroom");

	Fsm.CreateState(EMonsterState::BLINKING, std::bind(&AMushroom::Blinking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::INIT_WALKING, std::bind(&AMushroom::WalkingForStart, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::THINKING, std::bind(&AMushroom::Thinking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::WALKING, std::bind(&AMushroom::Walking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::DAMAGED, std::bind(&AMushroom::Damaging, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::DYING, std::bind(&AMushroom::Dying, this, std::placeholders::_1), std::bind(&AMushroom::OnDead, this));
	Fsm.CreateState(EMonsterState::PASS_AWAY, std::bind(&AMushroom::PassAwaing, this, std::placeholders::_1), std::bind(&AMushroom::OnPassaway, this));
	Fsm.CreateState(EMonsterState::JUMPING, std::bind(&AMushroom::Jumping, this, std::placeholders::_1));
}

AMushroom::~AMushroom()
{
}

void AMushroom::BeginPlay()
{
	AMonster::BeginPlay();

	InitSprite();
	InitCollision();
}

void AMushroom::Tick(float _deltaTime)
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

void AMushroom::InitSprite()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;
	const float animTime = 0.5f;

	SRBody = CreateDefaultSubObject<USpriteRenderer>();
	SRBody->SetSprite(SPRITE_NAME);
	SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SRBody->SetComponentScale(size);
	SRBody->SetPivotType(PivotType::Bot);
	SRBody->SetOrder(ERenderOrder::MONSTER);

	SRBody->CreateAnimation(ANIM_IDLE_UP, SPRITE_NAME, 0, 0, animTime);
	SRBody->CreateAnimation(ANIM_IDLE_DOWN, SPRITE_NAME, 22, 22, animTime);
	SRBody->CreateAnimation(ANIM_IDLE_LEFT, SPRITE_NAME, 33, 33, animTime);
	SRBody->CreateAnimation(ANIM_IDLE_RIGHT, SPRITE_NAME, 11, 11, animTime);

	SRBody->CreateAnimation(ANIM_RUN_UP, SPRITE_NAME, 1, 6, animTime);
	SRBody->CreateAnimation(ANIM_RUN_DOWN, SPRITE_NAME, 23, 28, animTime);
	SRBody->CreateAnimation(ANIM_RUN_LEFT, SPRITE_NAME, 34, 39, animTime);
	SRBody->CreateAnimation(ANIM_RUN_RIGHT, SPRITE_NAME, 12, 17, animTime);

	std::vector<int> idxs;
	std::vector<float> times;

	const int IDX_ANIM_START = 44;
	const int IDX_ANIM_END = 54;
	int jumpAnimSize = IDX_ANIM_END - IDX_ANIM_START;

	idxs.reserve(jumpAnimSize);
	times.reserve(jumpAnimSize);

	const int IDX_LONG_START = IDX_ANIM_START + 3;
	const int IDX_LONG_END = IDX_ANIM_END - 3;
	for (int i = IDX_ANIM_START; i < IDX_ANIM_END; i++)
	{
		idxs.push_back(i);
		if (i >= IDX_LONG_START && i < IDX_LONG_END)
		{
			times.push_back(1.f);
		}
		else
		{
			times.push_back(0.3f);
		}
		
	}

	SRBody->CreateAnimation(ANIM_JUMP, SPRITE_NAME, idxs, times, false);
	SetScore(EMonsterScore::S100);

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
		SRScore->SetSprite(MONSTER_SCORE_PATH);
		SRScore->SetComponentLocation(size.Half().Half() + FVector2D{ size.Half().Half().X + 1.f, 0.f });
		SRScore->SetComponentScale(size);
		SRScore->SetOrder(ERenderOrder::MONSTER_SCORE);

		std::vector<int> idxs;
		std::vector<float> times(SCORE_ANIM_CNT, .2f);
		idxs.reserve(SCORE_ANIM_CNT);
		times.resize(SCORE_ANIM_CNT);
		for (int i = 0; i < SCORE_ANIM_CNT; ++i)
		{
			idxs.push_back(i);
		}
		times[SCORE_ANIM_CNT - 2] = 1.f;

		SRScore->CreateAnimation(ANIM_SCORE, MONSTER_SCORE_PATH, idxs, times, false);
		SRScore->SetActive(false);
	}

	SRBody->SetActive(false);
}

void AMushroom::InitCollision()
{
	FVector2D collSize = GlobalVar::BOMB_SIZE;
	Collision = CreateDefaultSubObject<U2DCollision>();
	Collision->SetComponentLocation({ collSize.hX(), 0.f });
	Collision->SetComponentScale(collSize);
	Collision->SetCollisionGroup(ECollisionGroup::MonsterBody);
	Collision->SetCollisionType(ECollisionType::CirCle);
}

FVector2D AMushroom::GetMonsterSize()
{
	return SRBody->GetComponentScale();
}

FIntPoint AMushroom::GetDamageRange()
{
	return FIntPoint{ 1, 1 };
}

void AMushroom::ChangeMoveAnim(const FVector2D& _direction)
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

bool AMushroom::IsJump()
{
	bool isJump = Random.RandomInt(0, 5) == 0;	// ~16%
	EMonsterState nowState = static_cast<EMonsterState>(Fsm.GetState());

	if (isJump && nowState != EMonsterState::JUMPING)
	{
		return true;
	}

	return false;
}

void AMushroom::Jumping(float _deltaTime)
{
	SRBody->ChangeAnimation(ANIM_JUMP);

	if (SRBody->IsCurAnimationEnd())
	{
		Fsm.ChangeState(EMonsterState::THINKING);
	}
}

void AMushroom::Move(const FVector2D& _direction, float _deltaTime)
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

void AMushroom::Damaged(unsigned __int8 _power)
{
	if (static_cast<EMonsterState>(Fsm.GetState()) != EMonsterState::DAMAGED)
	{
		Health -= _power;
		Fsm.ChangeState(EMonsterState::DAMAGED);
	}
}

void AMushroom::Kill()
{
	if (static_cast<EMonsterState>(Fsm.GetState()) != EMonsterState::DYING)
	{
		if (Collision != nullptr)
		{
			Collision->SetActive(false);
		}
		Fsm.ChangeState(EMonsterState::DYING);
	}
}

void AMushroom::OnPause()
{
	if (SRBody)
	{
		SRBody->PauseCurAnimation();
	}
}

void AMushroom::OnResume()
{
	if (SRBody)
	{
		SRBody->ResumeCurAnimation();
	}
}

/* FSM start callbacks */
void AMushroom::OnDead()
{
	UEngineSound::Play(SFXDying);
	CanHit = false;
}

void AMushroom::OnPassaway()
{
	SRCloud->ChangeAnimation(ANIM_CLOUD, true);
	SRCloud->SetActive(true);

	SRScore->ChangeAnimation(ANIM_SCORE);
	SRScore->SetActive(true);

	GameData::GetInstance().AddPlayer1Score(GetScore());
}

/* FSM callbacks */
void AMushroom::Blinking(float _deltaTime)
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

void AMushroom::WalkingForStart(float _deltaTime)
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

void AMushroom::Thinking(float _deltaTime)
{
	//OutputDebugString(("PathFinderIdx: " + std::to_string(PathFinderIdx) + "\n").c_str());
	if (IsRouteEmpty())
	{
		FindPath();

		if (IsJump())
		{
			Fsm.ChangeState(EMonsterState::JUMPING);
		}
	}
	else
	{
		Fsm.ChangeState(EMonsterState::WALKING);
	}
}

void AMushroom::Walking(float _deltaTime)
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

void AMushroom::Damaging(float _deltaTime)
{
	// TODO: blinking..

	if (Health <= 0)
	{
		Kill();
	}
}

void AMushroom::Dying(float _deltaTime)
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

void AMushroom::PassAwaing(float _deltaTime)
{
	if (IsDestroiable) return;

	if (SRCloud->IsCurAnimationEnd() && SRScore->IsCurAnimationEnd())
	{
		IsDestroiable = true;
	}
}