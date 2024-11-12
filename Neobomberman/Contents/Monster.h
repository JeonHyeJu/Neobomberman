#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/PathFindAStar.h>

class AMonster : public AActor
{
public:
	AMonster();
	~AMonster();

	AMonster(const AMonster& _other) = delete;
	AMonster(AMonster&& _other) noexcept = delete;
	AMonster& operator=(const AMonster& _other) = delete;
	AMonster& operator=(AMonster&& _other) noexcept = delete;

	virtual void BeginPlay();
	virtual void Tick(float _deltaTime);
	virtual void Init() {};

	void SetCurMap(class APlayMap* _map);

protected:
	class USpriteRenderer* SpriteRenderer = nullptr;
	class APlayMap* CurMap = nullptr;

	float Speed = 1.f;
	UPathFindAStar PathFinder;
	std::list<FIntPoint> Route;
	FIntPoint Destination = { -1, -1 };
};
