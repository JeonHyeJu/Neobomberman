#pragma once
#include "BaseMap.h"
#include "ContentsStruct.h"

class APlayMap : public ABaseMap
{
public:
	APlayMap();
	~APlayMap();

	APlayMap(const APlayMap& _Other) = delete;
	APlayMap(APlayMap&& _Other) noexcept = delete;
	APlayMap& operator=(const APlayMap& _Other) = delete;
	APlayMap& operator=(APlayMap&& _Other) noexcept = delete;

	void InitMap();
	
protected:
	 void BeginPlay() override;
	 void Tick(float _deltaTime) override;

private:
	class USpriteRenderer* SRBackground = nullptr;
	class USpriteRenderer* SRBackgroundBottom = nullptr;
	class USpriteRenderer* SRLeftBear = nullptr;
	class USpriteRenderer* SRRightBear = nullptr;
};
