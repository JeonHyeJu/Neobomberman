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

	void InitMap();

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
};
