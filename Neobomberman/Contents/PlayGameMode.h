#pragma once
#include <EngineCore/GameMode.h>

class APlayGameMode : public AGameMode
{
public:
	APlayGameMode();
	~APlayGameMode();

	APlayGameMode(const APlayGameMode& _Other) = delete;
	APlayGameMode(APlayGameMode&& _Other) noexcept = delete;
	APlayGameMode& operator=(const APlayGameMode& _Other) = delete;
	APlayGameMode& operator=(APlayGameMode&& _Other) noexcept = delete;

	void BeginPlay();

protected:

private:
};

