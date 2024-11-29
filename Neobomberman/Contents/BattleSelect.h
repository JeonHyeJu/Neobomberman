#pragma once
#include <EngineCore/Actor.h>
#include <EngineBase/FSMStateManager.h>

class USpriteRenderer;
class ABattleSelect : public AActor
{
public:
	enum class ESceneState
	{
		SELECT_MAP = 0,
		WAITING_MAP_FADE,
		SELECT_CHARACTER,
		WAITING_CHARACTER_FADE,
		COMPLETE_SELECT,
	};

	ABattleSelect();
	~ABattleSelect();

	ABattleSelect(const ABattleSelect& _other) = delete;
	ABattleSelect(ABattleSelect&& _other) noexcept = delete;
	ABattleSelect& operator=(const ABattleSelect& _other) = delete;
	ABattleSelect& operator=(ABattleSelect&& _other) noexcept = delete;

private:
	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void ChangeToSelectCharacter();
	void ChangeToBattleScene();
	void ResetSeconds();
	void CountDown();

	void OnEndFadeOut();

	void OnSelectCharacter();

	void SelectingMap(float _deltaTime);
	void SelectingCharacter(float _deltaTime);
	void CompletingSelect(float _deltaTime);

	USpriteRenderer* SRBackground = nullptr;
	USpriteRenderer* SRSirenL = nullptr;
	USpriteRenderer* SRSirenR = nullptr;

	USpriteRenderer* SRSelectTimeCount[2] = { nullptr, nullptr };

	USpriteRenderer* SRCharacterWhite = nullptr;
	USpriteRenderer* SRCharacterBlack = nullptr;
	USpriteRenderer* SRCharacterRed = nullptr;
	USpriteRenderer* SRCharacterBlue = nullptr;

	USpriteRenderer* SRBalloonBBM = nullptr;

	USpriteRenderer* SRPlayer1Name = nullptr;
	USpriteRenderer* SRPlayer1Sign = nullptr;

	const char* SPRITE_SELECT_MAP = "SelectMap.png";
	const char* SPRITE_SELECT_CHARACTER = "SelectCharacter.png";
	const char* SPRITE_BALLOON_BBM = "BalloonBomberman.png";

	const char* SPRITE_SELECT_TIME_COUNT_B = "BlueCount.png";
	const char* SPRITE_SELECT_TIME_COUNT_P = "PinkCount.png";
	const char* SPRITE_SELECT_TIME_COUNT_Y = "YellowCount.png";

	const char* SPRITE_CHARACTER_WHITE = "SelectWhite.png";
	const char* SPRITE_CHARACTER_BLACK = "SelectBlack.png";
	const char* SPRITE_CHARACTER_RED = "SelectRed.png";
	const char* SPRITE_CHARACTER_BLUE = "SelectBlue.png";

	const char* SPRITE_CHARACTER_NAMES = "CharacterNames.png";
	const char* SPRITE_SIGN_1P = "Sign1p.png";

	const char* ANIM_HEAD_UP = "HeadUp";
	const char* ANIM_HEAD_DOWN = "HeadDown";
	const char* ANIM_TELEPORT_FORWARD = "Teleport_Forward";
	const char* ANIM_TELEPORT_BACKWARD = "Teleport_Backward";

	const int START_SECONDS = 30;
	int Seconds = START_SECONDS;

	UFSMStateManager Fsm;
	const int STRIDE = 96;

	int CurIdx = 0;
	int PreIdx = 0;

	/* Sounds */
	const char* SFXMenuSelect = "MenuSelect.mp3";
	const char* SFXMoveBBM = "MoveBBM.mp3";

	std::vector<USpriteRenderer*> Characters;
};
