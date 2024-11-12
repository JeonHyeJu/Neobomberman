#include "PreCompile.h"
#include "Mushroom.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "PlayMap.h"
#include "TileMap.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/PathFindAStar.h>

AMushroom::AMushroom()
{
	Speed = 20.f;
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
			State = EMushroomState::WALKING;
			return;
		}

		SpriteRenderer->SetActiveSwitch();
	}
	else if (State == EMushroomState::WALKING)
	{
		// Temp
		if (Route.empty())
		{
			FindPath();
		}
		else
		{
			if (Destination.X == -1 && Destination.Y == -1)
			{
				Destination = Route.front();
				Route.remove(Destination);
			}

			Walk(_deltaTime);
		}
	}
	else if (State == EMushroomState::JUMPING)
	{
		SpriteRenderer->ChangeAnimation("Jump");
	}
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

	State = EMushroomState::INIT_BLINK;
}

void AMushroom::Walk(float _deltaTime)
{
	static FIntPoint none = FIntPoint{ -1, -1 };

	FVector2D monsterLoc = GetActorLocation();
	FIntPoint monsterIdx = CurMap->GetGroundMap()->LocationToMatrixIdx(monsterLoc);

	if (monsterIdx == Destination)
	{
		Destination = none;
		return;
	}

	if (Destination == none)
	{
		return;
	}

	FVector2D direction = Destination - monsterIdx;
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

void AMushroom::FindPath()
{
	FVector2D playerLoc = GetWorld()->GetPawn()->GetActorLocation();
	FIntPoint playerIdx = CurMap->GetGroundMap()->LocationToMatrixIdx(playerLoc);
	FVector2D monsterLoc = GetActorLocation();
	FIntPoint monsterIdx = CurMap->GetGroundMap()->LocationToMatrixIdx(monsterLoc);

	Route = PathFinder.PathFind(monsterIdx, playerIdx);
}