#pragma once
#include <EngineCore/Actor.h>

class USpriteRenderer;
class AGameUI : public AActor
{
public:
	AGameUI();
	~AGameUI();

	AGameUI(const AGameUI& _other) = delete;
	AGameUI(AGameUI&& _other) noexcept = delete;
	AGameUI& operator=(const AGameUI& _other) = delete;
	AGameUI& operator=(AGameUI&& _other) noexcept = delete;

	void AddCoin(unsigned __int8 __addVal);
	void CheckCoin();
	unsigned __int8 GetCoin() const;

protected:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

private:
	void SetCoinSprite();

	USpriteRenderer* SRLevel_4 = nullptr;
	USpriteRenderer* SRCredits = nullptr;
	USpriteRenderer* SRCreditCount[2] = { nullptr, nullptr };

	const char* SPRITE_TIME_COUNT = "Title_countdown_24x24.png";
};
