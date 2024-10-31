#include "PreCompile.h"
#include "SceneComponent.h"

USceneComponent::USceneComponent()
{
}

USceneComponent::~USceneComponent()
{
}

FTransform USceneComponent::GetActorTransform()
{
	FTransform ActorTransform = GetActor()->GetTransform();
	FTransform Result;

	Result.Location = ActorTransform.Location + Transform.Location;
	Result.Scale = Transform.Scale;
	return Result;
}