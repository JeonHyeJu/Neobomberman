#pragma once
#include <EngineBase/Object.h>

class UActorComponent : public UObject
{
public:
	friend class AActor;

	typedef UActorComponent Super;

	UActorComponent();
	virtual ~UActorComponent() = 0;

	UActorComponent(const UActorComponent& _Other) = delete;
	UActorComponent(UActorComponent&& _Other) noexcept = delete;
	UActorComponent& operator=(const UActorComponent& _Other) = delete;
	UActorComponent& operator=(UActorComponent&& _Other) noexcept = delete;

	virtual void BeginPlay() {}
	virtual void ComponentTick(float _DeltaTime) {}

	class AActor* GetActor()
	{
		return ParentActor;
	}

	bool IsActive() override
	{
		return UObject::IsActive() && GetActor()->IsActive();
	}

	bool IsDestroy() override
	{
		return UObject::IsDestroy() || GetActor()->IsDestroy();
	}


protected:

private:
	class AActor* ParentActor;
};

