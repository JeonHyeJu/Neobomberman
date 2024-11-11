#pragma once
#include "Monster.h"

class AMushroom : public AMonster
{
public:
	AMushroom();
	~AMushroom();

	AMushroom(const AMushroom& _other) = delete;
	AMushroom(AMushroom&& _other) noexcept = delete;
	AMushroom& operator=(const AMushroom& _other) = delete;
	AMushroom& operator=(AMushroom&& _other) noexcept = delete;

	void Init(std::string_view _spritePath) override;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
};
