#pragma once
#include "Monster.h"

class ABalloon : public AMonster
{
public:
	ABalloon();
	~ABalloon();

	ABalloon(const ABalloon& _other) = delete;
	ABalloon(ABalloon&& _other) noexcept = delete;
	ABalloon& operator=(const ABalloon& _other) = delete;
	ABalloon& operator=(ABalloon&& _other) noexcept = delete;

	void Init(std::string_view _spritePath) override;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;
};