#pragma once
#include <EngineBase/EngineMath.h>
#include <list>
#include <vector>

class UPathFindNode
{
public:
	UPathFindNode* ParentNode;
	FIntPoint Point;
	float F = 0.0f;
	float G = 0.0f; // start pos ~ cur position
	float H = 0.0f; // cur position ~ finish pos

	FVector2D GetPointToVector()
	{
		return { Point.X, Point.Y };
	}
};

class IPathFindData
{
public:
	virtual bool IsMove(const FIntPoint& _Point) = 0;
};

class UPathFindAStar
{
public:
	UPathFindAStar();
	~UPathFindAStar();

	UPathFindAStar(const UPathFindAStar& _Other) = delete;
	UPathFindAStar(UPathFindAStar&& _Other) noexcept = delete;
	UPathFindAStar& operator=(const UPathFindAStar& _Other) = delete;
	UPathFindAStar& operator=(UPathFindAStar&& _Other) noexcept = delete;

	std::list<FIntPoint> PathFind(const FIntPoint& _Start, const FIntPoint& _End);

	void SetData(IPathFindData* _PathFindData)
	{
		PathFindData = _PathFindData;
	}

protected:

private:
	IPathFindData* PathFindData = nullptr;

	std::vector<FIntPoint> WayDir;

	std::list<UPathFindNode*> OpenList;
	std::list<UPathFindNode*> CloseList;

	FIntPoint EndPoint;

	std::vector<UPathFindNode> NodePool;
	int PoolCount = 0;

	void NodeClear()
	{
		OpenList.clear();
		CloseList.clear();
		PoolCount = 0;
	}

	bool FindOpenNode(FIntPoint _Point);
	bool FindCloseNode(FIntPoint _Point);

	UPathFindNode* GetNewNode(FIntPoint _Point, UPathFindNode* _ParentNode)
	{
		UPathFindNode* NewNode = &NodePool[PoolCount];
		NewNode->Point = _Point;
		NewNode->ParentNode = _ParentNode;

		FVector2D ThisPos = NewNode->GetPointToVector();

		if (nullptr != _ParentNode)
		{
			FVector2D ParentPos = _ParentNode->GetPointToVector();
			NewNode->G = _ParentNode->G + (ThisPos- ParentPos).Length();
		}

		FVector2D EndPos = { EndPoint.X, EndPoint.Y };
		NewNode->H = (EndPos - ThisPos).Length();
		NewNode->F = NewNode->H + NewNode->G;

		OpenList.push_back(NewNode);

		++PoolCount;
		return NewNode;
	}
};

