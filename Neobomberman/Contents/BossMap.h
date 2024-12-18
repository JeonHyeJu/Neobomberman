#pragma once
#include "BaseMap.h"
#include <EngineCore/PathFindAStar.h>
#include "ContentsStruct.h"

class ATileMap;
class ABossMap : public ABaseMap
{
public:
	ABossMap();
	~ABossMap();

	ABossMap(const ABossMap& _other) = delete;
	ABossMap(ABossMap&& _other) noexcept = delete;
	ABossMap& operator=(const ABossMap& _other) = delete;
	ABossMap& operator=(ABossMap&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void InitSprite() override;
	void InitTileMap() override;

private:
	const char* ANIM_RUN_BG = "Run";

	class USpriteRenderer* SRBackground = nullptr;
	class USpriteRenderer* SRBackgroundBottom = nullptr;
	class USpriteRenderer* SRLeftBear = nullptr;
	class USpriteRenderer* SRRightBear = nullptr;
};
