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

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void InitSprite() override;
	void InitTileMap() override;

private:
	class USpriteRenderer* SRBackground = nullptr;
	class USpriteRenderer* SRBgBuilding = nullptr;
};
