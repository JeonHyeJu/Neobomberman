#include "PreCompile.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "TileMap.h"
#include "PlayMap.h"
#include "Player.h"
#include "Bomb.h"

#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineCoreDebug.h>
#include <EngineCore/2DCollision.h>

#include <EnginePlatform/EngineInput.h>

APlayer::APlayer()
{
	FVector2D playerSize = GlobalVar::BOMBERMAN_SIZE;

	// Head: (32, 32), Center: (16, 16)
	// Real head size: 16 x 16
	// marginW (48) + character face picture space left (8) = 56
	// character face picture space up (8) + marginH (32) - size of head that pops out (16: whold head, 12: manual size) = 28
	// origianl : { 56, 16+32 } 
	SetActorLocation({ 112, 64 + 32 - 12 });	// temp

	{
		SpriteRendererHead = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererHead->SetSprite(PLAYER_SPRITE_PATH);
		SpriteRendererHead->SetComponentLocation({ 0, 0 });
		SpriteRendererHead->SetComponentScale(playerSize);
		SpriteRendererHead->SetPivotType(PivotType::Bot);

		SpriteRendererHead->CreateAnimation("Idle_Up", PLAYER_SPRITE_PATH, 17, 17, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Up", PLAYER_SPRITE_PATH, 18, 22, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Down", PLAYER_SPRITE_PATH, 0, 0, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Down", PLAYER_SPRITE_PATH, 1, 6, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Left", PLAYER_SPRITE_PATH, 9, 9, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Left", PLAYER_SPRITE_PATH, 10, 14, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Right", PLAYER_SPRITE_PATH, 24, 24, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Right", PLAYER_SPRITE_PATH, 25, 30, 0.1f);

		SpriteRendererHead->CreateAnimation("BlinkingEyes", PLAYER_SPRITE_PATH, 580, 581, 0.3f);
		SpriteRendererHead->CreateAnimation("Dead", PLAYER_SPRITE_PATH, 591, 600, 0.2f, false);
	}
	
	{
		SpriteRendererBody = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererBody->SetSprite(PLAYER_SPRITE_PATH);
		SpriteRendererBody->SetComponentLocation({ 0, static_cast<int>(playerSize.Y *.5f)});
		SpriteRendererBody->SetComponentScale(playerSize);
		SpriteRendererBody->SetPivotType(PivotType::Bot);

		SpriteRendererBody->CreateAnimation("Idle_Up", PLAYER_SPRITE_PATH, 48, 48, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Up", PLAYER_SPRITE_PATH, 49, 54, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Down", PLAYER_SPRITE_PATH, 32, 32, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Down", PLAYER_SPRITE_PATH, 33, 38, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Left", PLAYER_SPRITE_PATH, 40, 40, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Left", PLAYER_SPRITE_PATH, 41, 46, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Right", PLAYER_SPRITE_PATH, 56, 56, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Right", PLAYER_SPRITE_PATH, 57, 62, 0.1f);

		SpriteRendererBody->CreateAnimation("BlinkingEyes", PLAYER_SPRITE_PATH, 612, 613, 0.3f);
		SpriteRendererBody->CreateAnimation("Dead", PLAYER_SPRITE_PATH, 622, 631, 0.2f, false);
	}

	{
		FVector2D collSize = GlobalVar::BOMB_SIZE;

		Collision = CreateDefaultSubObject<U2DCollision>();
		Collision->SetComponentLocation({ 0.f, -collSize.hY()});
		Collision->SetComponentScale(collSize);
		Collision->SetCollisionGroup(ECollisionGroup::PlayerBody);
		Collision->SetCollisionType(ECollisionType::CirCle);

		Collision->SetCollisionEnter(std::bind(&APlayer::OnEnterCollision, this, std::placeholders::_1));

		GetWorld()->CollisionGroupLink(ECollisionGroup::PlayerBody, ECollisionGroup::MonsterBody);

		/*Collision->SetCollisionStay(std::bind(&ANewPlayer::CollisionStay, this, std::placeholders::_1));
		Collision->SetCollisionEnd(std::bind(&ANewPlayer::CollisionEnd, this, std::placeholders::_1));*/

		//DebugOn();
	}

	DyingAnimInfo.AnimSeconds = 1.5f;
	DyingAnimInfo.WaitSeconds = 3.0f;

	SpriteRendererHead->SetOrder(ERenderOrder::PLAYER);
	SpriteRendererBody->SetOrder(ERenderOrder::PLAYER);

	FsmH.CreateState(EPlayerState::REBORN, std::bind(&APlayer::Reborning, this, std::placeholders::_1), std::bind(&APlayer::OnReborn, this));
	FsmH.CreateState(EPlayerState::IDLE, std::bind(&APlayer::Idling, this, std::placeholders::_1), std::bind(&APlayer::OnIdle, this));
	FsmH.CreateState(EPlayerState::MOVE, std::bind(&APlayer::Moving, this, std::placeholders::_1));
	FsmH.CreateState(EPlayerState::DEAD, std::bind(&APlayer::Dying, this, std::placeholders::_1), std::bind(&APlayer::OnDead, this));
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	Super::BeginPlay();

	FsmH.ChangeState(EPlayerState::IDLE);
}

void APlayer::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	UEngineDebug::CoreOutPutString("FPS : " + std::to_string(1.0f / _deltaTime));
	//UEngineDebug::CoreOutPutString("PlayerPos : " + GetActorLocation().ToString());

	FsmH.Update(_deltaTime);

	EPlayerState nowState = static_cast<EPlayerState>(FsmH.GetState());
	if (nowState != EPlayerState::DEAD)
	{
		bool isDownSpace = UEngineInput::GetInst().IsDown(VK_SPACE);
		if (isDownSpace)
		{
			SPDropBomb = UEngineSound::Play("CreateBomb.mp3");
			DropBomb();
		}

		if (IsDownAnyKeyWithSetDir())
		{
			// Start to move
			FsmH.ChangeState(EPlayerState::MOVE);
			return;
		}
		else if (IsPressedAnyKey())
		{
			// Nothing..
		}
		else
		{
			FsmH.ChangeState(EPlayerState::IDLE);
			return;
		}
	}

	//FVector2D nowPos = GetActorLocation();
	//std::string nowPosStr = (std::to_string(nowPos.X) + ", " + std::to_string(nowPos.Y));
	//std::string additionalPosStr = (std::to_string(additionalPos.X) + ", " + std::to_string(additionalPos.Y) + "\n");
	//OutputDebugString((nowPosStr + " -> " + additionalPosStr).c_str());
}

bool APlayer::IsDownAnyKeyWithSetDir()
{
	bool isDownD = UEngineInput::GetInst().IsDown('D');
	bool isDownA = UEngineInput::GetInst().IsDown('A');
	bool isDownS = UEngineInput::GetInst().IsDown('S');
	bool isDownW = UEngineInput::GetInst().IsDown('W');

	if (isDownD)
	{
		Direction = FVector2D::RIGHT;
	}
	else if (isDownA)
	{
		Direction = FVector2D::LEFT;
	}
	else if (isDownS)
	{
		Direction = FVector2D::DOWN;
	}
	else if (isDownW)
	{
		Direction = FVector2D::UP;
	}

	bool ret = isDownD || isDownA || isDownS || isDownW;
	return ret;
}

bool APlayer::IsPressedAnyKey()
{
	bool isPressedD = UEngineInput::GetInst().IsPress('D');
	bool isPressedA = UEngineInput::GetInst().IsPress('A');
	bool isPressedS = UEngineInput::GetInst().IsPress('S');
	bool isPressedW = UEngineInput::GetInst().IsPress('W');

	bool ret = isPressedD || isPressedA || isPressedS || isPressedW;
	return ret;
}

std::string APlayer::GetDirectionStr()
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

void APlayer::DropBomb()
{
	FVector2D loc = GetActorLocation();
	FIntPoint idx = CurMap->GetGroundMap()->LocationToMatrixIdx(loc);
	if (ABomb::CanSetBombThisIdx(idx))
	{
		ABomb* pBomb = GetWorld()->SpawnActor<ABomb>();
		pBomb->Init(loc, Ability.BombType, Ability.Power, CurMap);
	}
}

void APlayer::OnEnterCollision(AActor* _actor)
{
	_actor->SetActive(false);	// temp
	FsmH.ChangeState(EPlayerState::DEAD);
}

/* FSM start functions */
void APlayer::OnReborn()
{
	SpriteRendererHead->SetActive(true);
	SpriteRendererBody->SetActive(true);

	Direction = FVector2D::ZERO;

	// Temp
	SpriteRendererHead->ChangeAnimation("Idle_Down");
	SpriteRendererBody->ChangeAnimation("Idle_Down");
}

void APlayer::OnIdle()
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

void APlayer::OnDead()
{
	SpriteRendererHead->ChangeAnimation("Dead");
	SpriteRendererBody->ChangeAnimation("Dead");
}

/* FSM update functions */
void APlayer::Reborning(float _deltaTime)
{
	static float allElapsedSecs = 0.f;		// Temp
	static float elapsedSecs = 0.f;		// TODO: change to local

	allElapsedSecs += _deltaTime;
	elapsedSecs += _deltaTime;

	if (allElapsedSecs >= 1.f)
	{
		SpriteRendererHead->SetActive(true);
		SpriteRendererBody->SetActive(true);

		FsmH.ChangeState(EPlayerState::IDLE);
		return;
	}

	if (elapsedSecs >= 0.1f)
	{
		elapsedSecs = 0.f;
		SpriteRendererHead->SetActiveSwitch();
		SpriteRendererBody->SetActiveSwitch();
	}
}

void APlayer::Idling(float _deltaTime)
{
	BlinkEyeAnimInfo.Seconds += _deltaTime;

	if (BlinkEyeAnimInfo.IsRunning)
	{
		if (BlinkEyeAnimInfo.Seconds >= BlinkEyeAnimInfo.AnimSeconds)
		{
			BlinkEyeAnimInfo.Seconds = 0.f;
			BlinkEyeAnimInfo.IsRunning = false;
			SpriteRendererHead->ChangeAnimation("Idle_Down");
			SpriteRendererBody->ChangeAnimation("Idle_Down");
		}
	}
	else
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

void APlayer::Moving(float _deltaTime)
{
	BlinkEyeAnimInfo.Seconds = 0.f;

	std::string suffixStr = GetDirectionStr();
	SpriteRendererHead->ChangeAnimation("Run_" + suffixStr);
	SpriteRendererBody->ChangeAnimation("Run_" + suffixStr);

	FVector2D additionalPos;
	if (Direction == FVector2D::LEFT)
	{
		additionalPos = Direction * 16.f;
	}
	else if (Direction == FVector2D::RIGHT)
	{
		additionalPos = Direction * 16.f;
	}
	else if (Direction == FVector2D::UP)
	{
		additionalPos = Direction * 9.f;
	}
	else if (Direction == FVector2D::DOWN)
	{
		additionalPos = Direction * 10.f;
	}

	FVector2D nextPos = GetActorLocation() + (Direction * _deltaTime * Ability.Speed) + additionalPos;

	// Temp
	const int POS_X_MIN = 96;	// 48
	const int POS_X_MAX = 512;	// 256
	const int POS_Y_MIN = 64 + 12;	// 32 + 8
	const int POS_Y_MAX = 416;	// 208 - 4;

	bool isMove = false;
	if (nextPos.X >= POS_X_MIN && nextPos.X < POS_X_MAX && nextPos.Y >= POS_Y_MIN && nextPos.Y < POS_Y_MAX)
	{
		isMove = true;
	}

	if (CurMap != nullptr)
	{
		ATileMap* wallMap = CurMap->GetWallMap();
		bool isMovableWall = wallMap->GetIsMovable(nextPos);

		ATileMap* boxMap = CurMap->GetBoxMap();
		bool isMovableBox = boxMap->GetIsMovable(nextPos);

		isMove = (isMove && isMovableWall && isMovableBox);
	}

	if (isMove)
	{
		AddActorLocation(Direction * _deltaTime * Ability.Speed);
	}
}

void APlayer::Dying(float _deltaTime)
{
	static bool isOff = false;
	static float elapsedSecs = 0.f;		// TODO: change to local

	if (SpriteRendererHead->IsCurAnimationEnd())
	{
		DyingAnimInfo.Seconds += _deltaTime;

		if (DyingAnimInfo.Seconds >= DyingAnimInfo.AnimSeconds)
		{
			// only once
			if (!isOff)
			{
				isOff = true;
				SpriteRendererHead->SetActive(false);
				SpriteRendererBody->SetActive(false);
			}

			if (DyingAnimInfo.Seconds >= DyingAnimInfo.WaitSeconds)
			{
				isOff = false;
				elapsedSecs = 0.f;
				DyingAnimInfo.Seconds = 0.f;

				FsmH.ChangeState(EPlayerState::REBORN);
				return;
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
