#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "GameData.h"
#include "Monster.h"
#include "PlayMap.h"
#include "TileMap.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/2DCollision.h>

int AMonster::MonsterCount = 0;
UPathFindAStar AMonster::PathFinder;

AMonster::AMonster()
: MonsterIdx(++AMonster::MonsterCount)
{
	{
		FVector2D collSize = GlobalVar::BOMB_SIZE;
		Collision = CreateDefaultSubObject<U2DCollision>();
		Collision->SetComponentLocation({ collSize.hX(), 0.f });
		Collision->SetComponentScale(collSize);
		Collision->SetCollisionGroup(ECollisionGroup::MonsterBody);
		Collision->SetCollisionType(ECollisionType::CirCle);
	}

	{
		FVector2D size = GlobalVar::BOMBERMAN_SIZE;
		SRCloud = CreateDefaultSubObject<USpriteRenderer>();
		SRCloud->SetSprite(CLOUD_SPRITE_PATH);
		SRCloud->SetComponentLocation(size.Half().Half().Half() + FVector2D{ 0, 16 });	// Temp
		SRCloud->SetComponentScale(size);
		SRCloud->CreateAnimation("Disappear", CLOUD_SPRITE_PATH, 0, 7, .2f, false);
		SRCloud->SetActive(false);
	}

	{
		FVector2D size = GlobalVar::BOMB_SIZE;
		SRScore = CreateDefaultSubObject<USpriteRenderer>();
		SRScore->SetSprite(MONSTER_SCORE_PATH);
		SRScore->SetComponentLocation(size.Half().Half());
		SRScore->SetComponentScale(size);

		std::vector<int> idxs;
		std::vector<float> times(SCORE_ANIM_CNT, .2f);
		idxs.reserve(SCORE_ANIM_CNT);
		times.resize(SCORE_ANIM_CNT);
		for (int i = 0; i < SCORE_ANIM_CNT; ++i)
		{
			idxs.push_back(i);
		}
		times[SCORE_ANIM_CNT - 2] = 1.f;

		SRScore->CreateAnimation("Disappear", MONSTER_SCORE_PATH, idxs, times, false);
		SRScore->SetActive(false);
	}

	Fsm.CreateState(EMonsterState::INIT_BLINK, std::bind(&AMonster::Blinking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::INIT_WALKING, std::bind(&AMonster::WalkingForStart, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::THINKING, std::bind(&AMonster::Thinking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::WALKING, std::bind(&AMonster::Walking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::DYING, std::bind(&AMonster::Dying, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::PASS_AWAY, std::bind(&AMonster::PassAwaing, this, std::placeholders::_1), std::bind(&AMonster::OnPassaway, this));

	//DebugOn();
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	// Init child's sprite
	InitSprite();

	assert(SRBody != nullptr && "You didn't initialize SRBody");
	SRBody->SetActive(false);
}

void AMonster::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	if (!IsInited)
	{
		static float accSecs = 0.f;
		accSecs += _deltaTime;
		if (accSecs >= StartDelayMs)
		{
			IsInited = true;
			Fsm.ChangeState(EMonsterState::INIT_BLINK);
			SRBody->SetActive(true);
		}

		return;	// TODO
	}

	PathFinderIdx = PathFinderIdx % AMonster::MonsterCount;
	PathFinderIdx++;

	Fsm.Update(_deltaTime);
}

void AMonster::OnPause()
{
	if (SRBody)
	{
		SRBody->PauseCurAnimation();
	}
}

void AMonster::OnResume()
{
	if (SRBody)
	{
		SRBody->ResumeCurAnimation();
	}
}

void AMonster::SetCurMap(ABaseMap* _map)
{
	CurMap = _map;
	PathFinder.SetData(CurMap);
}

void AMonster::SetFirstDestination(const FIntPoint& _idx)
{
	FirstIdx = _idx;
}

void AMonster::SetScore(EMonsterScore _score)
{
	Score = _score;
	//SRScore->SetSprite(MONSTER_SCORE_PATH);	// TODO
}

bool AMonster::IsRouteEmpty()
{
	return Route.empty();
}

bool AMonster::IsArrivedAtOneBlock()
{
	return Destination.X == -1 && Destination.Y == -1;
}

FVector2D AMonster::GetDirection(const FIntPoint& _vec)
{
	FVector2D direction = _vec;

	if (_vec.X < 0)
	{
		direction = FVector2D::LEFT;
	}
	else if (_vec.X > 0)
	{
		direction = FVector2D::RIGHT;
	}
	else if (_vec.Y < 0)
	{
		direction = FVector2D::UP;
	}
	else if (_vec.Y > 0)
	{
		direction = FVector2D::DOWN;
	}

	return direction;
}

void AMonster::Move(const FVector2D& _direction, float _deltaTime)
{
	ChangeMoveAnim(_direction);
	AddActorLocation(_direction * _deltaTime * Speed);
}

void AMonster::FindPath()
{
	if (PathFinderIdx != MonsterIdx) return;

	FVector2D playerLoc = GetWorld()->GetPawn()->GetActorLocation();
	FIntPoint playerIdx = CurMap->GetGroundMap()->LocationToMatrixIdx(playerLoc);
	FVector2D monsterLoc = GetActorLocation();
	FIntPoint monsterIdx = CurMap->GetGroundMap()->LocationToMatrixIdx(monsterLoc);

	Route = PathFinder.PathFind(monsterIdx, playerIdx);
	if (Route.empty())
	{
		Route = PathFinder.PathFindAnotherEdge(monsterIdx);
	}

	//DebugPrintFIntVector(Route, "Route");
}

void AMonster::Kill()
{
	if (static_cast<EMonsterState>(Fsm.GetState()) != EMonsterState::DYING)
	{
		Collision->SetActive(false);
		Fsm.ChangeState(EMonsterState::DYING);
	}
}

/* FSM start callbacks */
void AMonster::OnPassaway()
{
	SRCloud->ChangeAnimation("Disappear", true);
	SRCloud->SetActive(true);

	SRScore->ChangeAnimation("Disappear");
	SRScore->SetActive(true);

	GameData::GetInstance().AddPlayer1Score(GetScore());
}

/* FSM callbacks */
void AMonster::Blinking(float _deltaTime)
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

void AMonster::WalkingForStart(float _deltaTime)
{
	FVector2D firstLoc = CurMap->GetGroundMap()->MatrixIdxToLocation(FirstIdx);
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

void AMonster::Thinking(float _deltaTime)
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

void AMonster::Walking(float _deltaTime)
{
	if (IsRouteEmpty())
	{
		Fsm.ChangeState(EMonsterState::THINKING);
		return;
	}

	if (IsArrivedAtOneBlock())
	{
		Destination = Route.front();
		Route.pop_front();
	}

	FVector2D monsterRealLoc = GetActorLocation();
	FIntPoint monsterRealLocInt = monsterRealLoc.ConvertToPoint();
	FIntPoint monsterMatrixIdx = CurMap->GetGroundMap()->LocationToMatrixIdx(monsterRealLoc);

	FVector2D destRealLoc = CurMap->GetGroundMap()->MatrixIdxToLocation(Destination);
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

void AMonster::Dying(float _deltaTime)
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

void AMonster::PassAwaing(float _deltaTime)
{
	if (IsDestroiable) return;

	if (SRCloud->IsCurAnimationEnd() && SRScore->IsCurAnimationEnd())
	{
		IsDestroiable = true;
	}
}