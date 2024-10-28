#pragma once
#include <EngineCore/Actor.h>

class APlayer : public AActor
{
public:
	APlayer();
	~APlayer();

	APlayer(const APlayer& _other) = delete;
	APlayer(APlayer&& _other) noexcept = delete;
	APlayer& operator=(const APlayer& _other) = delete;
	APlayer& operator=(APlayer&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	void LeftMove(float _DeltaTime);
	void RightMove(float _DeltaTime);
	void UpMove(float _DeltaTime);
	void DownMove(float _DeltaTime);

protected:

private:
	float Speed = 100.0f;

};
