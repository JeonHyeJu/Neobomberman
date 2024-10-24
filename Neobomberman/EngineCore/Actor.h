#pragma once
#include <EngineBase/EngineMath.h>

class AActor
{
public:
	friend class ULevel;

	AActor();
	~AActor();

	AActor(const AActor& _Other) = delete;
	AActor(AActor&& _Other) noexcept = delete;
	AActor& operator=(const AActor& _Other) = delete;
	AActor& operator=(AActor&& _Other) noexcept = delete;

	virtual void BeginPlay() {}
	virtual void Tick() {}
	virtual void Render();

	class ULevel* GetWorld()
	{
		return World;
	}

	void SetActorLoaction(FVector2D _Location)
	{
		Location = _Location;
	}

	void AddActorLoaction(FVector2D _Direction)
	{
		Location += _Direction;
	}

	void SetActorScale(FVector2D _Scale)
	{
		Scale = _Scale;
	}

protected:

private:
	class ULevel* World = nullptr;

	FVector2D Location = FVector2D::ZERO;
	FVector2D Scale = FVector2D::ZERO;
};

