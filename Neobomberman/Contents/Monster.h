#pragma once
#include "EngineCore/Actor.h"

class AMonster : public AActor
{
public:
	AMonster();
	~AMonster();

	AMonster(const AMonster& _other) = delete;
	AMonster(AMonster&& _other) noexcept = delete;
	AMonster& operator=(const AMonster& _other) = delete;
	AMonster& operator=(AMonster&& _other) noexcept = delete;

	virtual void Init(std::string_view _spritePath);

protected:
	class USpriteRenderer* SpriteRenderer = nullptr;
};
