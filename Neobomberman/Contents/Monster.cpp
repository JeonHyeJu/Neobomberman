#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
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
	Fsm.CreateState(EMonsterState::INIT_BLINK, std::bind(&AMonster::Blinking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::INIT_WALKING, std::bind(&AMonster::WalkingForStart, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::THINKING, std::bind(&AMonster::Thinking, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::WALKING, std::bind(&AMonster::Walking, this, std::placeholders::_1));

	FVector2D collSize = GlobalVar::BOMB_SIZE;
	Collision = CreateDefaultSubObject<U2DCollision>();
	Collision->SetComponentLocation({ collSize.hX(), 0.f });
	Collision->SetComponentScale(collSize);		// Temp
	Collision->SetCollisionGroup(ECollisionGroup::MonsterBody);
	Collision->SetCollisionType(ECollisionType::CirCle);

	DebugOn();
}

AMonster::~AMonster()
{

}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	Init();
	SpriteRenderer->SetActive(false);
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
			SpriteRenderer->SetActive(true);
		}

		return;	// TODO
	}

	PathFinderIdx = PathFinderIdx % AMonster::MonsterCount;
	PathFinderIdx++;

	Fsm.Update(_deltaTime);
}

void AMonster::SetCurMap(APlayMap* _map)
{
	CurMap = _map;
	PathFinder.SetData(CurMap);
}

void AMonster::SetFirstDestination(const FIntPoint& _idx)
{
	FirstIdx = _idx;
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

// TODO
void AMonster::Move(const FVector2D& direction, float _deltaTime)
{
	if (direction == FVector2D::UP)
	{
		SpriteRenderer->ChangeAnimation("Run_Up");
	}
	else if (direction == FVector2D::DOWN)
	{
		SpriteRenderer->ChangeAnimation("Run_Down");
	}
	else if (direction == FVector2D::LEFT)
	{
		SpriteRenderer->ChangeAnimation("Run_Left");
	}
	else if (direction == FVector2D::RIGHT)
	{
		SpriteRenderer->ChangeAnimation("Run_Right");
	}

	AddActorLocation(direction * _deltaTime * Speed);
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

/* FSM callbacks */
void AMonster::Blinking(float _deltaTime)
{
	static float accumulatedSecs = 0.f;
	static float blinkElapsedSecs = 0.f;

	accumulatedSecs += _deltaTime;
	if (accumulatedSecs >= BLINK_SECONDS)
	{
		accumulatedSecs = 0.f;
		SpriteRenderer->SetActive(true);
		Fsm.ChangeState(EMonsterState::INIT_WALKING);
		return;
	}

	blinkElapsedSecs += _deltaTime;
	if (blinkElapsedSecs > 0.1f)
	{
		blinkElapsedSecs = 0.f;
		SpriteRenderer->SetActiveSwitch();
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
	if (Route.empty())
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
	if (Route.empty())
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