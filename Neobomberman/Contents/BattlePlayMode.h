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
	void Tick(float _deltaTime) override;

private:
	void OnExplodeBomb();
	void CheckAfterExplosion(float _deltaTime);

	class APlayer* Player = nullptr;
	class ABaseMap* CurMapPtr = nullptr;

	FIntPoint StartPoint = { 0, 0 };

	/* Sounds */
	const char* SFXBg = "BattleMusic.mp3";
	const char* SFXReadyGo = "ReadyGo.mp3";

	bool IsStarted = false;

	std::vector<FIntPoint> SplashTileIdxsBackup;
	bool IsSplashCheck = false;
};
