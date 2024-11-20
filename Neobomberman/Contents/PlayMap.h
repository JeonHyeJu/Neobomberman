#pragma once
#include "BaseMap.h"
#include <EngineCore/PathFindAStar.h>
#include "ContentsStruct.h"

class APlayMap : public ABaseMap, public IPathFindData
{
public:
	APlayMap();
	~APlayMap();

	APlayMap(const APlayMap& _Other) = delete;
	APlayMap(APlayMap&& _Other) noexcept = delete;
	APlayMap& operator=(const APlayMap& _Other) = delete;
	APlayMap& operator=(APlayMap&& _Other) noexcept = delete;

	void InitMap();
	bool IsMove(const FIntPoint& _Point) override;
	
protected:
	 void BeginPlay() override;
	 void Tick(float _deltaTime) override;

private:
};
