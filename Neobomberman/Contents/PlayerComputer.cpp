#include "PreCompile.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "PlayerComputer.h"
#include <EngineCore/SpriteRenderer.h>

APlayerComputer::APlayerComputer()
{
	/*DyingAnimInfo.AnimSeconds = 1.5f;
	DyingAnimInfo.WaitSeconds = 3.0f;

	Fsm.CreateState(EPlayerState::REBORN, nullptr, std::bind(&APlayer::OnReborn, this));
	Fsm.CreateState(EPlayerState::IDLE, std::bind(&APlayer::Idling, this, std::placeholders::_1), std::bind(&APlayer::OnIdle, this));
	Fsm.CreateState(EPlayerState::MOVE, std::bind(&APlayer::Moving, this, std::placeholders::_1), std::bind(&APlayer::OnMove, this));
	Fsm.CreateState(EPlayerState::DEAD, std::bind(&APlayer::Dying, this, std::placeholders::_1), std::bind(&APlayer::OnDead, this));
	Fsm.CreateState(EPlayerState::PORTAL, nullptr, std::bind(&APlayer::OnShift, this));
	Fsm.CreateState(EPlayerState::END, nullptr, std::bind(&APlayer::OnEnd, this));

	FsmH.CreateState(EPlayerBlinkAndColState::BLINK_ON_COL_OFF, std::bind(&APlayer::Blinking, this, std::placeholders::_1), std::bind(&APlayer::OnTurnOnBlink, this));
	FsmH.CreateState(EPlayerBlinkAndColState::BLINK_OFF_COL_ON, nullptr, std::bind(&APlayer::OnTurnOffBlink, this));*/
}

APlayerComputer::~APlayerComputer()
{
}

void APlayerComputer::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(StartLocation);
}

void APlayerComputer::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}

void APlayerComputer::InitSprite(std::string_view _sprite)
{
	FVector2D playerSize = GlobalVar::BOMBERMAN_SIZE;
	{
		SpriteRendererHead = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererHead->SetSprite(_sprite);
		SpriteRendererHead->SetComponentLocation(playerSize.Half().Half() + FVector2D{ -1.f, 6.f });
		SpriteRendererHead->SetComponentScale(playerSize);
		SpriteRendererHead->SetPivotType(PivotType::Bot);

		SpriteRendererHead->CreateAnimation("Idle_Up", _sprite, 17, 17, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Up", _sprite, 18, 22, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Down", _sprite, 0, 0, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Down", _sprite, 1, 6, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Left", _sprite, 9, 9, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Left", _sprite, 10, 14, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Right", _sprite, 24, 24, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Right", _sprite, 25, 30, 0.1f);

		const int BLINK_CNT = 5;
		std::vector<int> idxs{ 580, 581, 580, 581, 580 };
		std::vector<float> times(BLINK_CNT, .2f);
		idxs.resize(5);
		times[BLINK_CNT - 1] = 2.f;

		SpriteRendererHead->CreateAnimation("BlinkingEyes", _sprite, idxs, times);
		SpriteRendererHead->CreateAnimation("Dead", _sprite, 591, 600, 0.2f, false);
		SpriteRendererHead->CreateAnimation("Ride_Portal", _sprite, 583, 589, 0.1f, false);
		SpriteRendererHead->CreateAnimation("Victory", _sprite, 601, 604, 0.5f, false);
	}

	{
		SpriteRendererBody = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererBody->SetSprite(_sprite);
		SpriteRendererBody->SetComponentLocation(SpriteRendererHead->GetComponentLocation() + FVector2D{ 0.f, playerSize.hY() });
		SpriteRendererBody->SetComponentScale(playerSize);
		SpriteRendererBody->SetPivotType(PivotType::Bot);

		SpriteRendererBody->CreateAnimation("Idle_Up", _sprite, 48, 48, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Up", _sprite, 49, 54, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Down", _sprite, 32, 32, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Down", _sprite, 33, 38, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Left", _sprite, 40, 40, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Left", _sprite, 41, 46, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Right", _sprite, 56, 56, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Right", _sprite, 57, 62, 0.1f);

		SpriteRendererBody->CreateAnimation("BlinkingEyes", _sprite, 612, 613, 1.f);
		SpriteRendererBody->CreateAnimation("Dead", _sprite, 622, 631, 0.2f, false);
		SpriteRendererBody->CreateAnimation("Ride_Portal", _sprite, 615, 621, 0.1f, false);

		SpriteRendererBody->CreateAnimation("Victory", _sprite, 633, 636, 0.5f, false);
	}

	SpriteRendererHead->ChangeAnimation("Idle_Down");
	SpriteRendererBody->ChangeAnimation("Idle_Down");

	SpriteRendererHead->SetOrder(ERenderOrder::PLAYER);
	SpriteRendererBody->SetOrder(ERenderOrder::PLAYER);
}