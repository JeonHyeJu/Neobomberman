#include "PreCompile.h"
#include "Mushroom.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "PlayMap.h"
#include "TileMap.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/PathFindAStar.h>
#include "DebugLog.h"	// TODO: delete

AMushroom::AMushroom()
{
	Speed = 20.f;
	Random.SetSeed(time(nullptr));
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

	bool isOverSecond = false;
	AccumulatedSecs += _deltaTime;
	if (AccumulatedSecs >= BLINK_SECONDS)
	{
		AccumulatedSecs = 0.f;
		isOverSecond = true;
	}

	// TODO: change to FSM
	if (State == EMushroomState::INIT_BLINK)
	{
		if (isOverSecond)
		{
			SpriteRenderer->SetActive(true);
			State = EMushroomState::INIT_WALKING;
			return;
		}

		SpriteRenderer->SetActiveSwitch();
	}
	else if (State == EMushroomState::INIT_WALKING)
	{
		WalkForFirstLoc(_deltaTime);
	}
	else if (State == EMushroomState::WALKING)
	{
		// Temp
		if (Route.empty())
		{
			bool isJump = (Random.RandomInt(0, 4) == 0);	// 25%
			if (isJump && State != EMushroomState::JUMPING)
			{
				State = EMushroomState::JUMPING;
			}

			// TODO: 몬스터별 번갈아서 하도록!
			// TODO: 정리
			// TODO: collision
			FindPath();
		}
		else
		{
			if (Destination.X == -1 && Destination.Y == -1)
			{
				Destination = Route.front();
				Route.pop_front();
			}

			Walk(_deltaTime);
		}
	}
	else if (State == EMushroomState::JUMPING)
	{
		Jump();
	}
}

void AMushroom::SetFirstIndex(const FIntPoint& _idx)
{
	FirstIdx = _idx;
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

	// temp. TODO
	int startIdx = 44;
	int endIdx = 54;
	int subIdx = endIdx - startIdx;
	indexes.reserve(subIdx);
	times.reserve(subIdx);
	for (int i = startIdx; i < endIdx; i++)
	{
		indexes.push_back(i);
		if (i >= startIdx+3 && i < endIdx-3)
		{
			times.push_back(1.f);
		}
		else
		{
			times.push_back(0.3f);
		}
		
	}
	SpriteRenderer->CreateAnimation("Jump", SPRITE_NAME, indexes, times);
	SpriteRenderer->SetAnimationEvent("Jump", endIdx-1, std::bind(&AMushroom::OnEndJump, this));

	State = EMushroomState::INIT_BLINK;
}

void AMushroom::WalkForFirstLoc(float _deltaTime)
{
	FVector2D firstLoc = CurMap->GetGroundMap()->MatrixIdxToLocation(FirstIdx);
	FVector2D monsterRealLoc = GetActorLocation();

	// Temp
	FIntPoint firstLocInt{ static_cast<int>(firstLoc.X), static_cast<int>(firstLoc.Y) };
	FIntPoint monsterRealLocInt{ static_cast<int>(monsterRealLoc.X), static_cast<int>(monsterRealLoc.Y) };

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
		State = EMushroomState::WALKING;
	}
}

void AMushroom::Walk(float _deltaTime)
{
	static FIntPoint none = FIntPoint{ -1, -1 };

	FVector2D monsterRealLoc = GetActorLocation();
	FIntPoint monsterRealLocInt = FIntPoint{ static_cast<int>(monsterRealLoc.X), static_cast<int>(monsterRealLoc.Y) };
	FIntPoint monsterMatrixIdx = CurMap->GetGroundMap()->LocationToMatrixIdx(monsterRealLoc);

	FVector2D destRealLoc = CurMap->GetGroundMap()->MatrixIdxToLocation(Destination);
	FIntPoint destRealLocInt = FIntPoint{ static_cast<int>(destRealLoc.X), static_cast<int>(destRealLoc.Y) };
	FIntPoint destMatrixIdx = Destination;

	//OutputDebugString(("monsterRealLoc: " + std::to_string(monsterRealLoc.X) + ", " + std::to_string(monsterRealLoc.Y) + " ... " + ("destRealLoc: " + std::to_string(destRealLoc.X) + ", " + std::to_string(destRealLoc.Y) + "\n")).c_str());
	//OutputDebugString(("monsterMatrixIdx: " + std::to_string(monsterMatrixIdx.X) + ", " + std::to_string(monsterMatrixIdx.Y) + " ... " + ("destMatrixIdx: " + std::to_string(destMatrixIdx.X) + ", " + std::to_string(destMatrixIdx.Y) + "\n")).c_str());

	FVector2D direction;
	bool isGotLast = monsterRealLocInt.X == destRealLocInt.X && monsterRealLocInt.Y <= destRealLocInt.Y;

	if (monsterMatrixIdx == destMatrixIdx)
	{
		//OutputDebugString("monsterIdx == Destination\n");
		if (isGotLast)
		{
			//OutputDebugString("isGotLast\n");
			Destination = none;
			return;
		}
	}

	if (Destination == none)
	{
		return;
	}

	// Temp
	direction = destRealLocInt - monsterRealLocInt;
	//OutputDebugString(("monsterLoc: " + std::to_string(monsterLoc.X) + ", " + std::to_string(monsterLoc.Y) + " ... " + ("destLoc: " + std::to_string(destLoc.X) + ", " + std::to_string(destLoc.Y) + "\n")).c_str());

	if (direction.X < 0)
	{
		direction = FVector2D::LEFT;
	}
	else if (direction.X > 0)
	{
		direction = FVector2D::RIGHT;
	}
	else if (direction.Y < 0)
	{
		direction = FVector2D::UP;
	}
	else if (direction.Y > 0)
	{
		direction = FVector2D::DOWN;
	}

	Move(direction, _deltaTime);
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

	DebugPrintFIntVector(Route, "Route");
}

void AMushroom::Move(const FVector2D& direction, float _deltaTime)
{
	if (direction == FVector2D::UP)
	{
		//OutputDebugString("UP\n");
		SpriteRenderer->ChangeAnimation("Run_Up");
	}
	else if (direction == FVector2D::DOWN)
	{
		//OutputDebugString("DOWN\n");
		SpriteRenderer->ChangeAnimation("Run_Down");
	}
	else if (direction == FVector2D::LEFT)
	{
		//OutputDebugString("LEFT\n");
		SpriteRenderer->ChangeAnimation("Run_Left");
	}
	else if (direction == FVector2D::RIGHT)
	{
		//OutputDebugString("RIGHT\n");
		SpriteRenderer->ChangeAnimation("Run_Right");
	}

	AddActorLocation(direction * _deltaTime * Speed);
}

void AMushroom::Jump()
{
	SpriteRenderer->ChangeAnimation("Jump");
}

void AMushroom::OnEndJump()
{
	State = EMushroomState::WALKING;
}