#pragma once
#include "BaseMap.h"

class ABattleMap : public ABaseMap
{
public:
	ABattleMap();
	~ABattleMap();

	ABattleMap(const ABattleMap& _other) = delete;
	ABattleMap(ABattleMap&& _other) noexcept = delete;
	ABattleMap& operator=(const ABattleMap& _other) = delete;
	ABattleMap& operator=(ABattleMap&& _other) noexcept = delete;

	void InitMap();

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	const char* SPRITE_BATTLE_BG = "BattleBg.png";
	const char* SPRITE_BATTLE_BUILDING = "BattleBuilding.png";

	class USpriteRenderer* SRBackground = nullptr;
	class USpriteRenderer* SRBgBuilding = nullptr;
};
