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

	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	void CheckCheat();
	void CheckDeadMonster();
	void OnExplodeBomb();

	void FadeOut();
	void OnEndFadeOut();

	bool IsAllMonstersDead() const;

	std::list<class AMonster*> MonsterList;

	FIntPoint StartPoint = { 6, 8 };
	class AResult* ResultScene = nullptr;
	class APlayer* Player = nullptr;

	bool isShowingResult = false;

	class ABaseMap* CurMapPtr = nullptr;

	/* Sounds */
	const char* SFXBg = "Stage1Boss.mp3";
};
