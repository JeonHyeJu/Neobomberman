#pragma once
#include <EngineCore/Actor.h>

class ABullet : public AActor
{
public:
	ABullet();
	~ABullet();

	ABullet(const ABullet& _Other) = delete;
	ABullet(ABullet&& _Other) noexcept = delete;
	ABullet& operator=(const ABullet& _Other) = delete;
	ABullet& operator=(ABullet&& _Other) noexcept = delete;

protected:
	void Tick(float _DeltaTime) override;

private:

};

