#pragma once
#include "ActorComponent.h"

class USceneComponent : public UActorComponent
{
public:
	USceneComponent();
	~USceneComponent();

	USceneComponent(const USceneComponent& _Other) = delete;
	USceneComponent(USceneComponent&& _Other) noexcept = delete;
	USceneComponent& operator=(const USceneComponent& _Other) = delete;
	USceneComponent& operator=(USceneComponent&& _Other) noexcept = delete;

	FTransform GetTransform()
	{
		return Transform;
	}

	FTransform GetActorTransform();

	FVector2D GetComponentScale()
	{
		return Transform.Scale;
	}

	FVector2D GetComponentLocation()
	{
		return Transform.Location;
	}

	void SetComponentLocation(FVector2D _Location)
	{
		Transform.Location = _Location;
	}

	void SetComponentScale(FVector2D _Scale)
	{
		Transform.Scale = _Scale;
	}

protected:

private:
	FTransform Transform;
};

