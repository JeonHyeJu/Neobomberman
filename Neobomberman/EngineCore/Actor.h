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

	FTransform GetTransform()
	{
		return Transform;
	}

	FVector2D GetActorLocation()
	{
		return Transform.Location;
	}

	template<typename ComponentType>
	ComponentType* CreateDefaultSubObject()
	{
		ComponentType* NewComponent = new ComponentType();
		UActorComponent* ComponentPtr = dynamic_cast<UActorComponent*>(NewComponent);
		ComponentPtr->ParentActor = this;
		Components.push_back(NewComponent);
		ComponentList.push_back(NewComponent);
		return NewComponent;
	}
protected:

private:
	static void ComponentBeginPlay();

	static bool IsNewActorCreate;
	static std::list<class UActorComponent*> ComponentList;

	class ULevel* World = nullptr;
	FTransform Transform;

	std::list<class UActorComponent*> Components;
};

