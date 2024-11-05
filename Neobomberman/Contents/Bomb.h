#pragma once
#include <EngineCore/Actor.h>
#include "ContentsEnum.h"

class ABomb : public AActor
{
public:
	ABomb();
	~ABomb();

	ABomb(const ABomb& _other) = delete;
	ABomb(ABomb&& _other) noexcept = delete;
	ABomb& operator=(const ABomb& _other) = delete;
	ABomb& operator=(ABomb&& _other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	inline EBombType GetBombType() const
	{
		return BombType;
	}
	inline void SetBombType(const EBombType& _bombType)
	{
		BombType = _bombType;
	}

private:
	EBombType BombType = EBombType::PLAIN;

	const char* RESOURCE_PLAINBOMB_PATH = "BombOrg_16x16";	// 16x16
	const char* RESOURCE_REDBOMB_PATH = "BombRed_16x16";	// 16x16
	const float EXPLODE_SECONDS = 3.f;

	class USpriteRenderer* SpriteRenderer = nullptr;

	float AccumulatedSeconds = 0.f;
	int CurIndex = 0;

	void Explode();
};
