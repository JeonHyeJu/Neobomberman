#include "PreCompile.h"
#include "PathFindAStar.h"
#include <algorithm>

UPathFindAStar::UPathFindAStar()
{
	NodePool.resize(1000);

	// 4방향
	// WayDir = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };

	WayDir = { {0, 1}, {0, -1}, {1, 0}, {-1, 0},  {-1, 1}, {-1, -1}, {1, 1}, {1, -1} };

}

UPathFindAStar::~UPathFindAStar()
{
}

bool Comp(UPathFindNode* first, UPathFindNode* second)
{
	if (first->F < second->F)
	{
		return true;
	}
	else
	{
		return false;
	}
}



std::list<FIntPoint> UPathFindAStar::PathFind(const FIntPoint& _Start, const FIntPoint& _End)
{
	NodeClear();
	EndPoint = _End;
	GetNewNode(_Start, nullptr);

	UPathFindNode* ResultNode = nullptr;

	// 맨처음 원소를 리턴한다.
	while (true)
	{
		if (OpenList.empty())
		{
			// 도달 못함.
			break;
		}
		

		// 정렬기준을 정해주는 함수를 넣어주면 
		// 혹은 내부에 존재하는 녀석이 operator<가능한 값형이면 자동정렬을 해준다.
		OpenList.sort(Comp);



		UPathFindNode* CurNode = OpenList.front();
		// 가장 앞의 노드를 삭제하는 함수.
		OpenList.pop_front();
		CloseList.push_back(CurNode);

		FIntPoint CheckPoint = {0, 0};

		for (size_t i = 0; i < WayDir.size(); i++)
		{
			CheckPoint = CurNode->Point + WayDir[i];

			// 이동할수 없는 타일이면 넣으면 안된다.
			if (false == PathFindData->IsMove(CheckPoint))
			{
				continue;
			}

			// 클로즈 리스트에 들어있어도 
			if (true == FindCloseNode(CheckPoint))
			{
				continue;
			}

			// 오픈 리스트에 들어있어도 
			if (true == FindOpenNode(CheckPoint))
			{
				continue;
			}

			if (CheckPoint == EndPoint)
			{
				break;
			}

			GetNewNode(CheckPoint, CurNode);
		}
		
		// 목적지에 도달했다는 것이므로 종료한다.
		if (CheckPoint == EndPoint)
		{
			ResultNode = CurNode;
			break;
		}


	}

	std::list<FIntPoint> Result;

	// ResultNode 도착지점 바로 근처의 노드일것이다.
	if (nullptr != ResultNode)
	{
		while (nullptr != ResultNode)
		{
			Result.push_front(ResultNode->Point);

			ResultNode = ResultNode->ParentNode;
		}
	}
	


	return Result;

}



bool UPathFindAStar::FindOpenNode(FIntPoint _Point)
{
	std::list<UPathFindNode*>::iterator StartIter = OpenList.begin();
	std::list<UPathFindNode*>::iterator Enditer = OpenList.end();

	for (; StartIter != Enditer; ++StartIter)
	{
		if ((*StartIter)->Point == _Point)
		{
			return true;
		}
	}

	return false;
}

bool UPathFindAStar::FindCloseNode(FIntPoint _Point)
{
	std::list<UPathFindNode*>::iterator StartIter = CloseList.begin();
	std::list<UPathFindNode*>::iterator Enditer = CloseList.end();

	for (; StartIter != Enditer; ++StartIter)
	{
		if ((*StartIter)->Point == _Point)
		{
			return true;
		}
	}

	return false;

}