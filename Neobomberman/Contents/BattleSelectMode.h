#pragma once
#include <EngineCore/GameMode.h>

class ABattleSelectMode : public AGameMode
{
public:
	ABattleSelectMode();
	~ABattleSelectMode();

	ABattleSelectMode(const ABattleSelectMode& _other) = delete;
	ABattleSelectMode(ABattleSelectMode&& _other) noexcept = delete;
	ABattleSelectMode& operator=(const ABattleSelectMode& _other) = delete;
	ABattleSelectMode& operator=(ABattleSelectMode&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
};
