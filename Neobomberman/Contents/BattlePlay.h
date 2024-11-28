#pragma once
#include <EngineCore/Actor.h>

class ABattlePlay : public AActor
{
public:
	ABattlePlay();
	~ABattlePlay();

	ABattlePlay(const ABattlePlay& _other) = delete;
	ABattlePlay(ABattlePlay&& _other) noexcept = delete;
	ABattlePlay& operator=(const ABattlePlay& _other) = delete;
	ABattlePlay& operator=(ABattlePlay&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;
};
