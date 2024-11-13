#pragma once
#include <EngineBase/EngineMath.h>
#include <list>
#include <vector>

class IPathFindData
{
public:
	virtual bool IsMove(const FIntPoint& _Point) = 0;
};

class UPathFindNode
{
public:
	UPathFindNode* ParentNode = nullptr;
	FIntPoint Point;
	float F = 0.0f;
	float G = 0.0f; // start pos ~ cur position
	float H = 0.0f; // cur position ~ finish pos

	FVector2D GetPointToVector()
	{
		return { Point.X, Point.Y };
	}
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
	std::list<FIntPoint> PathFindAnotherEdge(const FIntPoint& _start);

	void SetData(IPathFindData* _mapPtr)
	{
		MapPtr = _mapPtr;
	}

private:
	void NodeClear();
	bool FindOpenNode(FIntPoint _Point);
	bool FindCloseNode(FIntPoint _Point);
	UPathFindNode* GetNewNode(FIntPoint _Point, UPathFindNode* _ParentNode);

	IPathFindData* MapPtr = nullptr;

	const std::vector<FIntPoint> WayDir;
	std::vector<UPathFindNode> NodePool;
	FIntPoint EndPoint;
	int PoolCount = 0;

	std::list<UPathFindNode*> OpenList;
	std::list<UPathFindNode*> CloseList;

	FIntPoint LeftTop;
	FIntPoint RightTop;
	FIntPoint LeftBottom;
	FIntPoint RightBottom;

	FIntPoint InitLeftTop = { 99999, 99999 };
	FIntPoint InitRightTop = { 0, 99999 };
	FIntPoint InitLeftBottom = { 99999, 0 };
	FIntPoint InitRightBottom = { 0, 0 };
};