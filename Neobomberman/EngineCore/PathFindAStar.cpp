#include "PreCompile.h"
#include "PathFindAStar.h"
#include <algorithm>

UPathFindAStar::UPathFindAStar()
{
	NodePool.resize(1000);

	WayDir = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
}

UPathFindAStar::~UPathFindAStar()
{
}

bool Compare(UPathFindNode* first, UPathFindNode* second)
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

	while (true)
	{
		if (OpenList.empty())
		{
			break;
		}

		OpenList.sort(Compare);

		UPathFindNode* CurNode = OpenList.front();

		OpenList.pop_front();
		CloseList.push_back(CurNode);

		FIntPoint CheckPoint = {0, 0};

		for (size_t i = 0; i < WayDir.size(); i++)
		{
			CheckPoint = CurNode->Point + WayDir[i];

			if (false == PathFindData->IsMove(CheckPoint))
			{
				continue;
			}

			if (true == FindCloseNode(CheckPoint))
			{
				continue;
			}

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
		
		if (CheckPoint == EndPoint)
		{
			ResultNode = CurNode;
			break;
		}
	}

	std::list<FIntPoint> Result;

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