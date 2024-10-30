#pragma once
#include <EngineBase/Object.h>
#include <EngineBase/EngineMath.h>

#include "EngineSprite.h"

class AActor : public UObject
{
public:
	typedef AActor Super;

	friend class ULevel;

	AActor();
	~AActor();

	AActor(const AActor& _Other) = delete;
	AActor(AActor&& _Other) noexcept = delete;
	AActor& operator=(const AActor& _Other) = delete;
	AActor& operator=(AActor&& _Other) noexcept = delete;

	virtual void BeginPlay() {}
	virtual void Tick(float _DeltaTime) {}
	virtual void Render();

	class ULevel* GetWorld()
	{
		return World;
	}

	void SetActorLocation(FVector2D _Location)
	{
		Transform.Location = _Location;
	}

	void AddActorLocation(FVector2D _Direction)
	{
		Transform.Location += _Direction;
	}

	void SetActorScale(FVector2D _Scale)
	{
		Transform.Scale = _Scale;
	}

	FVector2D GetActorLocation()
	{
		return Transform.Location;
	}

protected:

private:
	class ULevel* World = nullptr;

	FTransform Transform;


public:
	class UEngineSprite* Sprite = nullptr;
	int CurIndex = 0; 
	void SetSprite(std::string_view _Name, int _CurIndex = 0);
};

