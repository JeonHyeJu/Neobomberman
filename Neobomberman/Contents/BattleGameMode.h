#pragma once
#include <EngineCore/GameMode.h>

class ABattleGameMode : public AGameMode
{
public:
	ABattleGameMode();
	~ABattleGameMode();

	ABattleGameMode(const ABattleGameMode& _other) = delete;
	ABattleGameMode(ABattleGameMode&& _other) noexcept = delete;
	ABattleGameMode& operator=(const ABattleGameMode& _other) = delete;
	ABattleGameMode& operator=(ABattleGameMode&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
};
