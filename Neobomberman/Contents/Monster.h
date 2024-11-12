#pragma once
#include "EngineCore/Actor.h"

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

	void SetCurMap(class APlayMap* _map)
	{
		CurMap = _map;
	}

protected:
	class USpriteRenderer* SpriteRenderer = nullptr;
	class APlayMap* CurMap = nullptr;
};
