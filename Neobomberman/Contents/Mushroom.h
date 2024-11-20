#pragma once
#include "Monster.h"
#include <EngineBase/EngineRandom.h>

class AMushroom : public AMonster
{
public:
	AMushroom();
	~AMushroom();

	AMushroom(const AMushroom& _other) = delete;
	AMushroom(AMushroom&& _other) noexcept = delete;
	AMushroom& operator=(const AMushroom& _other) = delete;
	AMushroom& operator=(AMushroom&& _other) noexcept = delete;

	void Init() override;
	void ShowScore() override;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void Thinking(float _deltaTime) override;

private:
	bool IsJump();
	void Jumping(float _deltaTime);

	const char* SPRITE_NAME = "Mushroom.png";
	const int IDX_ANIM_START = 44;
	const int IDX_ANIM_END = 54;

	UEngineRandom Random;
};
