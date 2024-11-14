#include "PreCompile.h"
#include "Mushroom.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "PlayMap.h"
#include "TileMap.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/PathFindAStar.h>
//#include "DebugLog.h"	// TODO: delete

AMushroom::AMushroom()
{
	Speed = 20.f;

	FSM.CreateState(EMushroomState::INIT_BLINK, std::bind(&AMushroom::Blinking, this, std::placeholders::_1));
	FSM.CreateState(EMushroomState::INIT_WALKING, std::bind(&AMushroom::WalkingForStart, this, std::placeholders::_1));
	FSM.CreateState(EMushroomState::THINKING, std::bind(&AMushroom::Thinking, this, std::placeholders::_1));
	FSM.CreateState(EMushroomState::WALKING, std::bind(&AMushroom::Walking, this, std::placeholders::_1));
	FSM.CreateState(EMushroomState::JUMPING, std::bind(&AMushroom::Jumping, this, std::placeholders::_1));
}

AMushroom::~AMushroom()
{
}

void AMushroom::BeginPlay()
{
	AMonster::BeginPlay();
}

void AMushroom::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);

	FSM.UpdateState(_deltaTime);
}

void AMushroom::SetFirstIndex(const FIntPoint& _idx)
{
	FirstIdx = _idx;
	Random.SetSeed(FirstIdx.X);
}

void AMushroom::Init()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;
	const float animTime = 0.5f;

	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(SPRITE_NAME);
	SpriteRenderer->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SpriteRenderer->SetComponentScale(size);
	SpriteRenderer->SetPivotType(PivotType::Bot);
	SpriteRenderer->SetOrder(ERenderOrder::PLAYER);

	SpriteRenderer->CreateAnimation("Idle_Up", SPRITE_NAME, 0, 0, animTime);
	SpriteRenderer->CreateAnimation("Idle_Down", SPRITE_NAME, 22, 22, animTime);
	SpriteRenderer->CreateAnimation("Idle_Left", SPRITE_NAME, 33, 33, animTime);
	SpriteRenderer->CreateAnimation("Idle_Right", SPRITE_NAME, 11, 11, animTime);

	SpriteRenderer->CreateAnimation("Run_Up", SPRITE_NAME, 1, 6, animTime);
	SpriteRenderer->CreateAnimation("Run_Down", SPRITE_NAME, 23, 28, animTime);
	SpriteRenderer->CreateAnimation("Run_Left", SPRITE_NAME, 34, 39, animTime);
	SpriteRenderer->CreateAnimation("Run_Right", SPRITE_NAME, 12, 17, animTime);

	std::vector<int> indexes;
	std::vector<float> times;

	int jumpAnimSize = IDX_ANIM_END - IDX_ANIM_START;
	indexes.reserve(jumpAnimSize);
	times.reserve(jumpAnimSize);

	const int IDX_LONG_START = IDX_ANIM_START + 3;
	const int IDX_LONG_END = IDX_ANIM_END - 3;
	for (int i = IDX_ANIM_START; i < IDX_ANIM_END; i++)
	{
		indexes.push_back(i);
		if (i >= IDX_LONG_START && i < IDX_LONG_END)
		{
			times.push_back(1.f);
		}
		else
		{
			times.push_back(0.3f);
		}
		
	}
	SpriteRenderer->CreateAnimation("Jump", SPRITE_NAME, indexes, times, false);

	FSM.ChangeState(EMushroomState::INIT_BLINK);
}

void AMushroom::FindPath()
{
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

FVector2D AMushroom::GetDirection(const FIntPoint& _vec)
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

void AMushroom::Move(const FVector2D& direction, float _deltaTime)
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

bool AMushroom::IsJump()
{
	bool isJump = Random.RandomInt(0, 4) == 0;
	EMushroomState nowState = static_cast<EMushroomState>(FSM.GetState());

	if (isJump && nowState != EMushroomState::JUMPING)
	{
		return true;
	}

	return false;
}

bool AMushroom::IsArrivedAtOneBlock()
{
	return Destination.X == -1 && Destination.Y == -1;
}

void AMushroom::Blinking(float _deltaTime)
{
	static float accumulatedSecs = 0.f;

	accumulatedSecs += _deltaTime;
	if (accumulatedSecs >= BLINK_SECONDS)
	{
		accumulatedSecs = 0.f;
		SpriteRenderer->SetActive(true);
		FSM.ChangeState(EMushroomState::INIT_WALKING);
		return;
	}

	SpriteRenderer->SetActiveSwitch();
}

void AMushroom::WalkingForStart(float _deltaTime)
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
		FSM.ChangeState(EMushroomState::THINKING);
	}
}

void AMushroom::Thinking(float _deltaTime)
{
	if (Route.empty())
	{
		// TODO: 몬스터별 번갈아서 하도록!
		// TODO: collision
		FindPath();

		if (IsJump())
		{
			FSM.ChangeState(EMushroomState::JUMPING);
		}
	}
	else
	{
		FSM.ChangeState(EMushroomState::WALKING);
	}
}

void AMushroom::Walking(float _deltaTime)
{
	if (Route.empty())
	{
		FSM.ChangeState(EMushroomState::THINKING);
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

void AMushroom::Jumping(float _deltaTime)
{
	SpriteRenderer->ChangeAnimation("Jump");

	if (SpriteRenderer->IsCurAnimationEnd())
	{
		FSM.ChangeState(EMushroomState::THINKING);
	}
}