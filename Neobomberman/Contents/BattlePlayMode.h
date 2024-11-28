#pragma once
#include <EngineCore/GameMode.h>

class ABattlePlayMode : public AGameMode
{
public:
	ABattlePlayMode();
	~ABattlePlayMode();

	ABattlePlayMode(const ABattlePlayMode& _other) = delete;
	ABattlePlayMode(ABattlePlayMode&& _other) noexcept = delete;
	ABattlePlayMode& operator=(const ABattlePlayMode& _other) = delete;
	ABattlePlayMode& operator=(ABattlePlayMode&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
};
