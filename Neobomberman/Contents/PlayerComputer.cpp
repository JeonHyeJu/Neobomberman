#include "PreCompile.h"
#include "PlayerComputer.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "BaseMap.h"
#include "GameData.h"
#include "Player.h"
#include "Bomb.h"
#include "TileMap.h"
#include <EngineCore/SpriteRenderer.h>
#include <EnginePlatform/EngineSound.h>

std::string APlayerComputer::SpritePath = "MainCharater_Black.png";

APlayerComputer::APlayerComputer()
{
	DamageMargin = URect(10, 10, 10, 10);
	DamageSize = GlobalVar::BOMB_SIZE;

	DyingAnimInfo.AnimSeconds = 1.5f;
	DyingAnimInfo.WaitSeconds = 3.0f;

	Fsm.CreateState(EPlayerComputerState::REBORN, nullptr, std::bind(&APlayerComputer::OnReborn, this));
	Fsm.CreateState(EPlayerComputerState::IDLE, std::bind(&APlayerComputer::Idling, this, std::placeholders::_1), std::bind(&APlayerComputer::OnIdle, this));
	Fsm.CreateState(EPlayerComputerState::THINKING, std::bind(&APlayerComputer::Thinking, this, std::placeholders::_1));
	Fsm.CreateState(EPlayerComputerState::MOVE, std::bind(&APlayerComputer::Moving, this, std::placeholders::_1), std::bind(&APlayerComputer::OnMove, this));
	Fsm.CreateState(EPlayerComputerState::DEAD, std::bind(&APlayerComputer::Dying, this, std::placeholders::_1), std::bind(&APlayerComputer::OnDead, this));
	Fsm.CreateState(EPlayerComputerState::END, nullptr, std::bind(&APlayerComputer::OnEnd, this));

	FsmH.CreateState(EBlinkAndColState::BLINK_ON_COL_OFF, std::bind(&APlayerComputer::Blinking, this, std::placeholders::_1), std::bind(&APlayerComputer::OnTurnOnBlink, this));
	FsmH.CreateState(EBlinkAndColState::BLINK_OFF_COL_ON, nullptr, std::bind(&APlayerComputer::OnTurnOffBlink, this));
}

APlayerComputer::~APlayerComputer()
{
}

void APlayerComputer::BeginPlay()
{
	Super::BeginPlay();

	PrevRouteIdx = MapPtr->LocationToMatrixIdx(StartLocation);
	Destination = PrevRouteIdx;
	Reborn();
}

void APlayerComputer::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	static float elpasedSecs = 0.f;	// Temp

	elpasedSecs += _deltaTime;
	if (elpasedSecs >= 3.f)
	{
		elpasedSecs = 0.f;
		static bool temp = true;
		if (temp)
		{
			temp = false;
			Fsm.ChangeState(EPlayerComputerState::THINKING);	// Temp
			return;
		}
	}

	Fsm.Update(_deltaTime);
	FsmH.Update(_deltaTime);
}

void APlayerComputer::InitSprite(std::string_view _sprite)
{
	if (_sprite.size() > 0)
	{
		SpritePath = _sprite;
	}

	FVector2D playerSize = GlobalVar::BOMBERMAN_SIZE;
	{
		SpriteRendererHead = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererHead->SetSprite(SpritePath);
		SpriteRendererHead->SetComponentLocation(playerSize.Half().Half() + FVector2D{ -1.f, 6.f });
		SpriteRendererHead->SetComponentScale(playerSize);
		SpriteRendererHead->SetPivotType(PivotType::Bot);

		SpriteRendererHead->CreateAnimation("Idle_Up", SpritePath, 17, 17, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Up", SpritePath, 18, 22, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Down", SpritePath, 0, 0, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Down", SpritePath, 1, 6, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Left", SpritePath, 9, 9, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Left", SpritePath, 10, 14, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Right", SpritePath, 24, 24, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Right", SpritePath, 25, 30, 0.1f);

		const int BLINK_CNT = 5;
		std::vector<int> idxs{ 580, 581, 580, 581, 580 };
		std::vector<float> times(BLINK_CNT, .2f);
		idxs.resize(5);
		times[BLINK_CNT - 1] = 2.f;

		SpriteRendererHead->CreateAnimation("BlinkingEyes", SpritePath, idxs, times);
		SpriteRendererHead->CreateAnimation("Dead", SpritePath, 591, 598, 0.2f, false);
		SpriteRendererHead->CreateAnimation("Ride_Portal", SpritePath, 583, 589, 0.1f, false);
		SpriteRendererHead->CreateAnimation("Victory", SpritePath, 601, 604, 0.5f, false);
	}

	{
		SpriteRendererBody = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererBody->SetSprite(SpritePath);
		SpriteRendererBody->SetComponentLocation(SpriteRendererHead->GetComponentLocation() + FVector2D{ 0.f, playerSize.hY() });
		SpriteRendererBody->SetComponentScale(playerSize);
		SpriteRendererBody->SetPivotType(PivotType::Bot);

		SpriteRendererBody->CreateAnimation("Idle_Up", SpritePath, 48, 48, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Up", SpritePath, 49, 54, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Down", SpritePath, 32, 32, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Down", SpritePath, 33, 38, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Left", SpritePath, 40, 40, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Left", SpritePath, 41, 46, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Right", SpritePath, 56, 56, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Right", SpritePath, 57, 62, 0.1f);

		SpriteRendererBody->CreateAnimation("BlinkingEyes", SpritePath, 612, 613, 1.f);
		SpriteRendererBody->CreateAnimation("Dead", SpritePath, 622, 629, 0.2f, false);
		SpriteRendererBody->CreateAnimation("Ride_Portal", SpritePath, 615, 621, 0.1f, false);

		SpriteRendererBody->CreateAnimation("Victory", SpritePath, 633, 636, 0.5f, false);
	}

	SpriteRendererHead->ChangeAnimation("Idle_Down");
	SpriteRendererBody->ChangeAnimation("Idle_Down");

	SpriteRendererHead->SetOrder(ERenderOrder::MONSTER);
	SpriteRendererBody->SetOrder(ERenderOrder::MONSTER);
}

void APlayerComputer::SetCurMap(ABaseMap* _map)
{
	MapPtr = _map;
	//PathFinder.SetData(CurMapPtr);
}

void APlayerComputer::Reborn()
{
	Fsm.ChangeState(EPlayerComputerState::REBORN);
	FsmH.ChangeState(EBlinkAndColState::BLINK_ON_COL_OFF);

	ResetDroppedBombs();
}

void APlayerComputer::ResetDroppedBombs()
{
	DroppedBombs.clear();
}

void APlayerComputer::Kill()
{
	if (!IsImmotal)
	{
		Fsm.ChangeState(EPlayerComputerState::DEAD);
	}
}

std::string APlayerComputer::GetDirectionStr()
{
	std::string suffixStr = "";
	if (Direction == FVector2D::LEFT)
	{
		suffixStr = "Left";
	}
	else if (Direction == FVector2D::RIGHT)
	{
		suffixStr = "Right";
	}
	else if (Direction == FVector2D::UP)
	{
		suffixStr = "Up";
	}
	else if (Direction == FVector2D::DOWN)
	{
		suffixStr = "Down";
	}

	return suffixStr;
}

/* Fsm start functions */
void APlayerComputer::OnReborn()
{
	SpriteRendererHead->SetActive(true);
	SpriteRendererBody->SetActive(true);

	Direction = FVector2D::ZERO;
	IsDead = false;

	SpriteRendererHead->ChangeAnimation("Idle_Down");
	SpriteRendererBody->ChangeAnimation("Idle_Down");
}

void APlayerComputer::OnIdle()
{
	std::string suffixStr = "";
	if (Direction == FVector2D::ZERO)
	{
		suffixStr = "Down";
	}
	else
	{
		suffixStr = GetDirectionStr();
	}

	SpriteRendererHead->ChangeAnimation("Idle_" + suffixStr);
	SpriteRendererBody->ChangeAnimation("Idle_" + suffixStr);
}

void APlayerComputer::OnMove()
{
}

void APlayerComputer::OnDead()
{
	GameData::GetInstance().AddPlayer2Life(-1);
	UEngineSound::Play(SFXDying);

	SpriteRendererHead->ChangeAnimation("Dead");
	SpriteRendererBody->ChangeAnimation("Dead");
}

void APlayerComputer::OnEnd()
{
	SpriteRendererHead->ChangeAnimation("Idle_Down");
	SpriteRendererBody->ChangeAnimation("Idle_Down");
}

/* FSM update functions */
void APlayerComputer::Idling(float _deltaTime)
{
	BlinkEyeAnimInfo.Seconds += _deltaTime;

	if (!BlinkEyeAnimInfo.IsRunning)
	{
		if (BlinkEyeAnimInfo.Seconds >= BlinkEyeAnimInfo.WaitSeconds)
		{
			BlinkEyeAnimInfo.Seconds = 0.f;
			BlinkEyeAnimInfo.IsRunning = true;
			SpriteRendererHead->ChangeAnimation("BlinkingEyes");
			SpriteRendererBody->ChangeAnimation("BlinkingEyes");
		}
	}
}

void APlayerComputer::Moving(float _deltaTime)
{
	if (Direction != FIntPoint::ZERO)
	{
		float RealSpeed = DEFAULT_SPEED + Ability.Speed * 25.f;

		FVector2D move = Direction* _deltaTime* RealSpeed;

		std::string suffixStr = GetDirectionStr();
		SpriteRendererHead->ChangeAnimation("Run_" + suffixStr);
		SpriteRendererBody->ChangeAnimation("Run_" + suffixStr);

		//AddActorLocation(Direction * 0.01f);
		AddActorLocation(Direction * _deltaTime * RealSpeed);
	}
}

void APlayerComputer::Thinking(float _deltaTime)
{
	//Fsm.ChangeState(EPlayerComputerState::MOVE);
}

void APlayerComputer::Dying(float _deltaTime)
{
	static bool isOff = false;
	static float elapsedSecs = 0.f;		// TODO: change to local

	if (SpriteRendererHead->IsCurAnimationEnd())
	{
		DyingAnimInfo.Seconds += _deltaTime;

		if (DyingAnimInfo.Seconds >= DyingAnimInfo.AnimSeconds)
		{
			IsDead = true;

			// only once
			if (!isOff)
			{
				isOff = true;
				SpriteRendererHead->SetActive(false);
				SpriteRendererBody->SetActive(false);
			}

			int life = GameData::GetInstance().GetPlayer1Life();
			if (life >= 0)
			{
				if (DyingAnimInfo.Seconds >= DyingAnimInfo.WaitSeconds)
				{
					isOff = false;
					elapsedSecs = 0.f;
					DyingAnimInfo.Seconds = 0.f;

					Reborn();
					return;
				}
			}
		}
		else
		{
			elapsedSecs += _deltaTime;
			if (elapsedSecs >= 0.1f)
			{
				elapsedSecs = 0.f;
				SpriteRendererHead->SetActiveSwitch();
				SpriteRendererBody->SetActiveSwitch();
			}
		}
	}
}

/* FsmH start functions */
void APlayerComputer::OnTurnOnBlink()
{
	IsImmotal = true;
}

void APlayerComputer::OnTurnOffBlink()
{
	IsImmotal = false;
}

/* FsmH update functions */
void APlayerComputer::Blinking(float _deltaTime)
{
	static float allElapsedSecs = 0.f;		// Temp
	static float elapsedSecs = 0.f;		// TODO: change to local

	allElapsedSecs += _deltaTime;
	elapsedSecs += _deltaTime;

	if (allElapsedSecs >= BLINK_SECS)
	{
		allElapsedSecs = 0.f;
		elapsedSecs = 0.f;

		SpriteRendererHead->SetActive(true);
		SpriteRendererBody->SetActive(true);

		FsmH.ChangeState(EBlinkAndColState::BLINK_OFF_COL_ON);
		return;
	}

	if (elapsedSecs >= 0.1f)
	{
		elapsedSecs = 0.f;
		SpriteRendererHead->SetActiveSwitch();
		SpriteRendererBody->SetActiveSwitch();
	}
}