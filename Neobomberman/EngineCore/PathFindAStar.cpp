#include "PreCompile.h"
#include "PathFindAStar.h"

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

UPathFindAStar::UPathFindAStar()
: WayDir({ {0, 1}, {0, -1}, {1, 0}, {-1, 0} })
{
	NodePool.resize(1000);
	Random.SetSeed(time(nullptr));
}

UPathFindAStar::~UPathFindAStar()
{
}

void UPathFindAStar::NodeClear()
{
	OpenList.clear();
	CloseList.clear();
	PoolCount = 0;

	LeftTop = InitLeftTop;
	RightTop = InitRightTop;
	LeftBottom = InitLeftBottom;
	RightBottom = InitRightBottom;
}

UPathFindNode* UPathFindAStar::GetNewNode(FIntPoint _Point, UPathFindNode* _ParentNode)
{
	UPathFindNode* NewNode = &NodePool[PoolCount];
	NewNode->Point = _Point;
	NewNode->ParentNode = _ParentNode;

	FVector2D ThisPos = NewNode->GetPointToVector();

	if (nullptr != _ParentNode)
	{
		FVector2D ParentPos = _ParentNode->GetPointToVector();
		NewNode->G = _ParentNode->G + (ThisPos - ParentPos).Length();
	}

	FVector2D EndPos = { EndPoint.X, EndPoint.Y };
	NewNode->H = (EndPos - ThisPos).Length();
	NewNode->F = NewNode->H + NewNode->G;

	OpenList.push_back(NewNode);

	++PoolCount;
	return NewNode;
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

			if (false == MapPtr->IsMove(CheckPoint))
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

	if (ResultNode != nullptr)
	{
		Result.push_back(EndPoint);
	}
	while (ResultNode != nullptr)
	{
		Result.push_front(ResultNode->Point);
		ResultNode = ResultNode->ParentNode;
	}

	return Result;
}

std::list<FIntPoint> UPathFindAStar::PathFindAnotherEdge(const FIntPoint& _start)
{
	std::list<FIntPoint> Result;

	if (CloseList.empty())
	{
		return Result;
	}

	std::list<UPathFindNode*>::iterator it = CloseList.begin();
	std::list<UPathFindNode*>::iterator itEnd = CloseList.end();

	for (; it != itEnd; ++it)
	{
		const FIntPoint& pt = (*it)->Point;

		if (pt.X <= LeftTop.X && pt.Y <= LeftTop.Y)
		{
			LeftTop = pt;
		}
		if (pt.X >= RightTop.X && pt.Y <= RightTop.Y)
		{
			RightTop = pt;
		}
		if (pt.X <= LeftBottom.X && pt.Y >= LeftBottom.Y)
		{
			LeftBottom = pt;
		}
		if (pt.X >= RightBottom.X && pt.Y >= RightBottom.Y)
		{
			RightBottom = pt;
		}
	}

	std::vector<FIntPoint> edges;
	edges.reserve(4);

	if (LeftTop != InitLeftTop)
	{
		edges.push_back(LeftTop);
	}
	if (RightTop != InitRightTop)
	{
		edges.push_back(RightTop);
	}
	if (LeftBottom != InitLeftBottom)
	{
		edges.push_back(LeftBottom);
	}
	if (RightBottom != InitRightBottom)
	{
		edges.push_back(RightBottom);
	}

	if (edges.empty())
	{
		return Result;
	}

	int val = Random.RandomInt(0, static_cast<int>(edges.size())-1);
	FIntPoint endPoint = edges[val];

	std::string vectorStr = "";
	for (size_t i = 0; i < edges.size(); i++)
	{
		vectorStr += "(" + std::to_string(edges[i].Y) + ", " + std::to_string(edges[i].X) + ")";
	}
	vectorStr += "\n";

	Result = PathFind(_start, endPoint);
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