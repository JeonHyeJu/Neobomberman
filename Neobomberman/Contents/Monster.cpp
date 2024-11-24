#include "PreCompile.h"
#include "GlobalVar.h"
#include "Monster.h"
#include "BaseMap.h"

int AMonster::MonsterCount = 0;
UPathFindAStar AMonster::PathFinder;

AMonster::AMonster()
: MonsterIdx(++AMonster::MonsterCount)
{
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();
}

void AMonster::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	// Is this my turn to find my way?
	PathFinderIdx = PathFinderIdx % MonsterCount;
	PathFinderIdx++;
}

void AMonster::OnPause()
{
	Super::OnPause();
}

void AMonster::OnResume()
{
	Super::OnResume();
}

void AMonster::SetCurMap(ABaseMap* _map)
{
	CurMap = _map;
	PathFinder.SetData(CurMap);
}

void AMonster::SetScore(EMonsterScore _score)
{
	Score = _score;
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

void AMonster::FindPath()
{
	if (PathFinderIdx != MonsterIdx) return;

	FVector2D playerLoc = GetWorld()->GetPawn()->GetActorLocation();
	FIntPoint playerIdx = CurMap->LocationToMatrixIdx(playerLoc);
	FVector2D monsterLoc = GetActorLocation();
	FIntPoint monsterIdx = CurMap->LocationToMatrixIdx(monsterLoc);

	Route = PathFinder.PathFind(monsterIdx, playerIdx);
	if (Route.empty())
	{
		Route = PathFinder.PathFindAnotherEdge(monsterIdx);
	}

	//DebugPrintFIntVector(Route, "Route");
}