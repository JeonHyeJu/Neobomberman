#pragma once
#include <EngineCore/GameMode.h>

class APlayBossMode : public AGameMode
{
public:
	APlayBossMode();
	~APlayBossMode();

	APlayBossMode(const APlayBossMode& _other) = delete;
	APlayBossMode(APlayBossMode&& _other) noexcept = delete;
	APlayBossMode& operator=(const APlayBossMode& _other) = delete;
	APlayBossMode& operator=(APlayBossMode&& _other) noexcept = delete;

	void BeginPlay();
};
