#pragma once
#include <EngineCore/Actor.h>

class USpriteRenderer;
class ASelectMap : public AActor
{
public:
	ASelectMap();
	~ASelectMap();

	ASelectMap(const ASelectMap& _other) = delete;
	ASelectMap(ASelectMap&& _other) noexcept = delete;
	ASelectMap& operator=(const ASelectMap& _other) = delete;
	ASelectMap& operator=(ASelectMap&& _other) noexcept = delete;

private:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	USpriteRenderer* SRBackground = nullptr;
	USpriteRenderer* SRSirenL = nullptr;
	USpriteRenderer* SRSirenR = nullptr;
};
