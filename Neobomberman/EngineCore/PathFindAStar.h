#pragma once
#include <EngineBase/EngineMath.h>
#include <list>
#include <vector>

// 철저하게 타일맵에 특화된 길찾기를 만들겠습니다.
// 대각선이동이 된다고 판단하고 만들겠습니다.
// 

class UPathFindNode
{
public:
	UPathFindNode* ParentNode;
	FIntPoint Point;
	float F = 0.0f; // G + H
	float G = 0.0f; // 현재 내 위치에서 시작지점과의 거리
	float H = 0.0f; // 현재 내 위치에서 도착지점과의 거리

	FVector2D GetPointToVector()
	{
		return { Point.X, Point.Y };
	}
};

class IPathFindData
{
public:
	// 일반적
	// const FIntPoint& _Point 위치가 이동할수 있는 지역이야?
	virtual bool IsMove(const FIntPoint& _Point) = 0;
};

// 설명 :
class UPathFindAStar
{
public:
	// constrcuter destructer
	UPathFindAStar();
	~UPathFindAStar();

	// delete Function
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

	// 풀 방식을 사용하겠습니다.
	// 미리 왕창 땡겨놓고 
	// 한계를 지정하고 더 만들거나 지우거나 삭제하지 않는 방식입니다.
	// 값형이기 때문에 메모리가 딱붙을수 있고.
	// 사용이 쉽기 때문에 미리 왕창 만들어 놓을 것이다.
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

