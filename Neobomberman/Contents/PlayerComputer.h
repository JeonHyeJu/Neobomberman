#pragma once
#include <EngineCore/Actor.h>

class APlayerComputer : public AActor
{
public:
	APlayerComputer();
	~APlayerComputer();

	APlayerComputer(const APlayerComputer& _other) = delete;
	APlayerComputer(APlayerComputer&& _other) noexcept = delete;
	APlayerComputer& operator=(const APlayerComputer& _other) = delete;
	APlayerComputer& operator=(APlayerComputer&& _other) noexcept = delete;

	void InitSprite(std::string_view _sprite);
	void SetStartLoc(const FVector2D& _val)
	{
		StartLocation = _val;
	}

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	class USpriteRenderer* SpriteRendererHead = nullptr;
	class USpriteRenderer* SpriteRendererBody = nullptr;

	FVector2D StartLocation;
};
