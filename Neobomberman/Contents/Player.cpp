#include "PreCompile.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include "GameData.h"
#include "GameUI.h"
#include "TileMap.h"
#include "BaseMap.h"
#include "Player.h"
#include "Result.h"
#include "Bomb.h"
#include "Fade.h"

#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineCoreDebug.h>
#include <EngineCore/2DCollision.h>
#include <EnginePlatform/EngineSound.h>
#include <EnginePlatform/EngineInput.h>

APlayer::APlayer()
{
	DamageMargin = URect(10, 10, 10, 10);
	DamageSize = GlobalVar::BOMB_SIZE;

	FVector2D size = GlobalVar::BOMBERMAN_SIZE;
	{
		SpriteRendererHead = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererHead->SetSprite(PLAYER_SPRITE_PATH);
		SpriteRendererHead->SetComponentLocation(size.Half().Half() + FVector2D{ -1.f, 6.f });
		SpriteRendererHead->SetComponentScale(size);
		SpriteRendererHead->SetPivotType(PivotType::Bot);

		SpriteRendererHead->CreateAnimation("Idle_Up", PLAYER_SPRITE_PATH, 17, 17, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Up", PLAYER_SPRITE_PATH, 18, 22, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Down", PLAYER_SPRITE_PATH, 0, 0, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Down", PLAYER_SPRITE_PATH, 1, 6, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Left", PLAYER_SPRITE_PATH, 9, 9, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Left", PLAYER_SPRITE_PATH, 10, 14, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Right", PLAYER_SPRITE_PATH, 24, 24, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Right", PLAYER_SPRITE_PATH, 25, 30, 0.1f);

		const int BLINK_CNT = 5;
		std::vector<int> idxs{ 580, 581, 580, 581, 580 };
		std::vector<float> times(BLINK_CNT, .2f);
		idxs.resize(5);
		times[BLINK_CNT - 1] = 2.f;

		SpriteRendererHead->CreateAnimation("BlinkingEyes", PLAYER_SPRITE_PATH, idxs, times);
		SpriteRendererHead->CreateAnimation("Dead", PLAYER_SPRITE_PATH, 591, 600, 0.2f, false);
		SpriteRendererHead->CreateAnimation("Ride_Portal", PLAYER_SPRITE_PATH, 583, 589, 0.1f, false);
		SpriteRendererHead->CreateAnimation("Victory", PLAYER_SPRITE_PATH, 601, 604, 0.5f, false);

		SpriteRendererHead->SetAnimationEvent("Ride_Portal", 589, std::bind(&APlayer::OnEndPortalAnim, this));
	}
	
	{
		SpriteRendererBody = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererBody->SetSprite(PLAYER_SPRITE_PATH);
		SpriteRendererBody->SetComponentLocation(SpriteRendererHead->GetComponentLocation() + FVector2D{ 0.f, size.hY() });
		SpriteRendererBody->SetComponentScale(size);
		SpriteRendererBody->SetPivotType(PivotType::Bot);

		SpriteRendererBody->CreateAnimation("Idle_Up", PLAYER_SPRITE_PATH, 48, 48, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Up", PLAYER_SPRITE_PATH, 49, 54, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Down", PLAYER_SPRITE_PATH, 32, 32, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Down", PLAYER_SPRITE_PATH, 33, 38, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Left", PLAYER_SPRITE_PATH, 40, 40, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Left", PLAYER_SPRITE_PATH, 41, 46, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Right", PLAYER_SPRITE_PATH, 56, 56, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Right", PLAYER_SPRITE_PATH, 57, 62, 0.1f);

		SpriteRendererBody->CreateAnimation("BlinkingEyes", PLAYER_SPRITE_PATH, 612, 613, 1.f);
		SpriteRendererBody->CreateAnimation("Dead", PLAYER_SPRITE_PATH, 622, 631, 0.2f, false);
		SpriteRendererBody->CreateAnimation("Ride_Portal", PLAYER_SPRITE_PATH, 615, 621, 0.1f, false);

		SpriteRendererBody->CreateAnimation("Victory", PLAYER_SPRITE_PATH, 633, 636, 0.5f, false);
	}

	{
		FVector2D collSize = GlobalVar::BOMB_SIZE;

		Collision = CreateDefaultSubObject<U2DCollision>();
		Collision->SetComponentLocation(collSize.Half());
		Collision->SetComponentScale(collSize);
		Collision->SetCollisionGroup(ECollisionGroup::PlayerBody);
		Collision->SetCollisionType(ECollisionType::CirCle);

		Collision->SetCollisionEnter(std::bind(&APlayer::OnEnterCollision, this, std::placeholders::_1));
		Collision->SetCollisionEnd(std::bind(&APlayer::OnEndCollision, this, std::placeholders::_1));

		GetWorld()->CollisionGroupLink(ECollisionGroup::PlayerBody, ECollisionGroup::MonsterBody);
		GetWorld()->CollisionGroupLink(ECollisionGroup::PlayerBody, ECollisionGroup::Bomb);

		/*Collision->SetCollisionStay(std::bind(&ANewPlayer::CollisionStay, this, std::placeholders::_1));
		Collision->SetCollisionEnd(std::bind(&ANewPlayer::CollisionEnd, this, std::placeholders::_1));*/
	}

	DyingAnimInfo.AnimSeconds = 1.5f;
	DyingAnimInfo.WaitSeconds = 3.0f;

	SpriteRendererHead->SetOrder(ERenderOrder::PLAYER);
	SpriteRendererBody->SetOrder(ERenderOrder::PLAYER);

	Fsm.CreateState(EPlayerState::REBORN, nullptr, std::bind(&APlayer::OnReborn, this));
	Fsm.CreateState(EPlayerState::IDLE, std::bind(&APlayer::Idling, this, std::placeholders::_1), std::bind(&APlayer::OnIdle, this));
	Fsm.CreateState(EPlayerState::MOVE, std::bind(&APlayer::Moving, this, std::placeholders::_1), std::bind(&APlayer::OnMove, this));
	Fsm.CreateState(EPlayerState::DEAD, std::bind(&APlayer::Dying, this, std::placeholders::_1), std::bind(&APlayer::OnDead, this));
	Fsm.CreateState(EPlayerState::PORTAL, nullptr, std::bind(&APlayer::OnShift, this));
	Fsm.CreateState(EPlayerState::END, nullptr, std::bind(&APlayer::OnEnd, this));

	FsmH.CreateState(EPlayerBlinkAndColState::BLINK_ON_COL_OFF, std::bind(&APlayer::Blinking, this, std::placeholders::_1), std::bind(&APlayer::OnTurnOnBlink, this));
	FsmH.CreateState(EPlayerBlinkAndColState::BLINK_OFF_COL_ON, nullptr, std::bind(&APlayer::OnTurnOffBlink, this));

	//DebugOn();
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	Super::BeginPlay();

	Reborn();
}

void APlayer::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	UEngineDebug::CoreOutPutString("FPS : " + std::to_string(1.0f / _deltaTime));
	//UEngineDebug::CoreOutPutString("PlayerPos : " + GetActorLocation().ToString());

	Fsm.Update(_deltaTime);
	FsmH.Update(_deltaTime);

	EPlayerState nowState = static_cast<EPlayerState>(Fsm.GetState());
	if (nowState != EPlayerState::MOVE)
	{
		UEngineSound::StopPlayer(SFXWalking);
	}

	// state != DEAD and PORTAL
	if (nowState < EPlayerState::DEAD)
	{
		CheckItem(_deltaTime);

		bool isDownSpace = UEngineInput::GetInst().IsDown(VK_SPACE);
		if (isDownSpace)
		{
			DropBomb();
		}

		if (IsDownAnyKeyWithSetDir())
		{
			// Start to move
			Fsm.ChangeState(EPlayerState::MOVE);
			return;
		}
		else if (IsPressedAnyKey())
		{
			// Nothing..
		}
		else
		{
			Fsm.ChangeState(EPlayerState::IDLE);
		}
	}

	//FVector2D nowPos = GetActorLocation();
	//std::string nowPosStr = (std::to_string(nowPos.X) + ", " + std::to_string(nowPos.Y));
	//std::string additionalPosStr = (std::to_string(additionalPos.X) + ", " + std::to_string(additionalPos.Y) + "\n");
	//OutputDebugString((nowPosStr + " -> " + additionalPosStr).c_str());
}

void APlayer::LevelChangeStart()
{
	IsClear = false;
	IsDead = false;
}

void APlayer::ReleaseMove()
{
	Fsm.ChangeState(EPlayerState::IDLE);
}

void APlayer::BlockMove()
{
	Fsm.ChangeState(EPlayerState::END);
}

void APlayer::OnResume()
{
	// Temp
	DyingAnimInfo.Seconds = DyingAnimInfo.WaitSeconds;
}

void APlayer::ShowWinnerPose()
{
	IsBossStage = true;
	UEngineSound::Play(SFXWin);

	Fsm.ChangeState(EPlayerState::PORTAL);
}

void APlayer::AddItem(EItem _item)
{
	switch (_item)
	{
	case EItem::BOMB:
		if (Ability.BombCount < GlobalVar::MAX_BOMB_CNT)
		{
			Ability.BombCount++;
		}
		break;
	case EItem::SPEED:
		if (Ability.Speed < GlobalVar::MAX_SPEED)
		{
			Ability.Speed++;
		}
		break;
	case EItem::POWER:
		if (Ability.Power < GlobalVar::MAX_BOMB_POWER)
		{
			Ability.Power++;
		}
		break;
	default:
		break;
	}
}

void APlayer::CheckItem(float _deltaTime)
{
	if (CurMapPtr == nullptr) return;

	static float elaspedSecs = 0.f;
	elaspedSecs += _deltaTime;

	if (elaspedSecs >= .5f) {
		float RealSpeed = DEFAULT_SPEED + Ability.Speed * 25.f;

		FVector2D curLoc = GetActorLocation();
		FVector2D nextPosLT = curLoc + (Direction * _deltaTime * RealSpeed) + FVector2D{ 6, 6 };
		FVector2D nextPosRT = curLoc + (Direction * _deltaTime * RealSpeed) + FVector2D{ 26, 6 };
		FVector2D nextPosLB = curLoc + (Direction * _deltaTime * RealSpeed) + FVector2D{ 6, 26 };
		FVector2D nextPosRB = curLoc + (Direction * _deltaTime * RealSpeed) + FVector2D{ 26, 26 };

		_CheckItem(nextPosLT);
		_CheckItem(nextPosRT);
		_CheckItem(nextPosLB);
		_CheckItem(nextPosRB);
	}
}

void APlayer::Reborn()
{
	Fsm.ChangeState(EPlayerState::REBORN);
	FsmH.ChangeState(EPlayerBlinkAndColState::BLINK_ON_COL_OFF);

	ResetDroppedBombs();
	SetActorLocation(StartLocation);
}

void APlayer::ResetDroppedBombs()
{
	DroppedBombs.clear();
}

bool APlayer::IsDownAnyKeyWithSetDir()
{
	bool isDownRight = UEngineInput::GetInst().IsDown(VK_RIGHT);
	bool isDownLeft = UEngineInput::GetInst().IsDown(VK_LEFT);
	bool isDownDown = UEngineInput::GetInst().IsDown(VK_DOWN);
	bool isDownUp = UEngineInput::GetInst().IsDown(VK_UP);

	if (isDownRight)
	{
		Direction = FVector2D::RIGHT;
	}
	else if (isDownLeft)
	{
		Direction = FVector2D::LEFT;
	}
	else if (isDownDown)
	{
		Direction = FVector2D::DOWN;
	}
	else if (isDownUp)
	{
		Direction = FVector2D::UP;
	}

	bool ret = isDownRight || isDownLeft || isDownDown || isDownUp;
	return ret;
}

bool APlayer::IsPressedAnyKey()
{
	bool isPressedRight = UEngineInput::GetInst().IsPress(VK_RIGHT);
	bool isPressedLeft = UEngineInput::GetInst().IsPress(VK_LEFT);
	bool isPressedDown = UEngineInput::GetInst().IsPress(VK_DOWN);
	bool isPressedUp = UEngineInput::GetInst().IsPress(VK_UP);

	bool ret = isPressedRight || isPressedLeft || isPressedDown || isPressedUp;
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
	FVector2D loc = GetActorLocation() + FVector2D{ 16, 16 };
	FIntPoint idx = CurMapPtr->LocationToMatrixIdx(loc);
	int curBombCnt = ABomb::GetBombCnt();
	if (ABomb::CanSetBombThisIdx(idx) && curBombCnt < Ability.BombCount)
	{
		ERenderOrder order = static_cast<ERenderOrder>(static_cast<int>(ERenderOrder::BOMB_5) - curBombCnt);

		ABomb* pBomb = GetWorld()->SpawnActor<ABomb>();
		pBomb->Init(loc, Ability.BombType, Ability.Power, CurMapPtr, order);

		DroppedBombs.push_back(pBomb);
		UEngineSound::Play(SFXDropBomb, 100);
	}
}

void APlayer::Kill()
{
	if (!IsImmotal)
	{
		Fsm.ChangeState(EPlayerState::DEAD);
	}
}

void APlayer::OnEnterCollision(AActor* _actor)
{
	if (_actor->GetName() != "Bomb")
	{
		Kill();
	}
}

void APlayer::OnEndCollision(AActor* _actor)
{
	std::list<ABomb*>::iterator it = DroppedBombs.begin();
	std::list<ABomb*>::iterator itEnd = DroppedBombs.end();

	for (; it != itEnd; ++it)
	{
		ABomb* pBomb = *it;
		if (pBomb == nullptr)
		{
			continue;
		}

		if (pBomb == _actor)
		{
			pBomb->SetIsMovableForPlayer(false);
			it = DroppedBombs.erase(it);
			if (it == itEnd) break;
		}
	}
}

void APlayer::OnEndPortalAnim()
{
	IsClear = true;
}

/* FSM2 start functions */
void APlayer::OnTurnOnBlink()
{
	IsImmotal = true;
}

void APlayer::OnTurnOffBlink()
{
	IsImmotal = false;
}

/* FSM start functions */
void APlayer::OnReborn()
{
	AGameUI::ResetTimer();
	AGameUI::StartTimer();

	SpriteRendererHead->SetActive(true);
	SpriteRendererBody->SetActive(true);

	Direction = FVector2D::ZERO;
	IsDead = false;

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

void APlayer::OnMove()
{
	
}

void APlayer::OnDead()
{
	GameData::GetInstance().AddPlayer1Life(-1);
	UEngineSound::Play(SFXDying);

	SpriteRendererHead->ChangeAnimation("Dead");
	SpriteRendererBody->ChangeAnimation("Dead");

	AGameUI::StopTimer();
}

void APlayer::OnShift()
{
	if (IsBossStage)
	{
		SpriteRendererHead->ChangeAnimation("Victory");
		SpriteRendererBody->ChangeAnimation("Victory");
	}
	else
	{
		UEngineSound::Play(SFXRidePortal);
		SpriteRendererHead->ChangeAnimation("Ride_Portal");
		SpriteRendererBody->ChangeAnimation("Ride_Portal");
	}
}

void APlayer::OnEnd()
{
	SpriteRendererHead->ChangeAnimation("Idle_Down");
	SpriteRendererBody->ChangeAnimation("Idle_Down");
}

/* FSM update functions */
void APlayer::Blinking(float _deltaTime)
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

		FsmH.ChangeState(EPlayerBlinkAndColState::BLINK_OFF_COL_ON);
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

void APlayer::_CheckItem(const FVector2D& _loc)
{
	FIntPoint nextIdx = CurMapPtr->LocationToMatrixIdx(_loc);
	if (CurMapPtr->HasItem(nextIdx))
	{
		EItem item = CurMapPtr->PopItem(nextIdx);
		AddItem(item);
		UEngineSound::Play(SFXItem, -1, 0, false);
	}
}

void APlayer::Moving(float _deltaTime)
{
	float RealSpeed = DEFAULT_SPEED + Ability.Speed * 25.f;

	BlinkEyeAnimInfo.Seconds = 0.f;
	BlinkEyeAnimInfo.IsRunning = false;

	std::string suffixStr = GetDirectionStr();
	SpriteRendererHead->ChangeAnimation("Run_" + suffixStr);
	SpriteRendererBody->ChangeAnimation("Run_" + suffixStr);

	FVector2D curLoc = GetActorLocation();
	FVector2D nextPosLT = curLoc + (Direction * _deltaTime * RealSpeed) + FVector2D{ 1, 1 };
	FVector2D nextPosRT = curLoc + (Direction * _deltaTime * RealSpeed) + FVector2D{ 31, 1 };
	FVector2D nextPosLB = curLoc + (Direction * _deltaTime * RealSpeed) + FVector2D{ 1, 31 };
	FVector2D nextPosRB = curLoc + (Direction * _deltaTime * RealSpeed) + FVector2D{ 31, 31 };

	// Temp
	const int POS_X_MIN = 97;
	const int POS_X_MAX = 512;	// 256
	const int POS_Y_MIN = 94;
	const int POS_Y_MAX = 416;	// 208 - 4;

	bool isMove = false;
	bool isBlockedByPortal = false;
	if (nextPosLT.X >= POS_X_MIN && nextPosRT.X < POS_X_MAX && nextPosLB.Y >= POS_Y_MIN && nextPosLB.Y < POS_Y_MAX)
	{
		isMove = true;
	}

	if (CurMapPtr != nullptr)
	{
		//bool canMoveMap = CurMapPtr->CanMove(nextPos);
		//isMove = isMove && canMoveMap;
		bool canMoveMapLT = CurMapPtr->CanMove(nextPosLT, true);
		bool canMoveMapRT = CurMapPtr->CanMove(nextPosRT, true);
		bool canMoveMapLB = CurMapPtr->CanMove(nextPosLB, true);
		bool canMoveMapRB = CurMapPtr->CanMove(nextPosRB, true);
		isMove = isMove && canMoveMapLB && canMoveMapRB && canMoveMapLT && canMoveMapRT;

		/*OutputDebugString(std::string(canMoveMapLT ? "canMoveMapLT\n" : "NLT....").c_str());
		OutputDebugString(std::string(canMoveMapRT ? "canMoveMapRT\n" : "NRT\n").c_str());
		OutputDebugString(std::string(canMoveMapLB ? "canMoveMapLB\n" : "NLB...").c_str());
		OutputDebugString(std::string(canMoveMapRB ? "canMoveMapRB\n" : "NRB\n").c_str());*/

		FIntPoint portalIdx = CurMapPtr->GetPortalIdx();

		// Portal
		if (CurMapPtr->GetIsPortalOpened())
		{
			FIntPoint curIdx = CurMapPtr->LocationToMatrixIdx(nextPosLB);
			if (curIdx == portalIdx)
			{
				// Move to orginized location
				FVector2D organizedLoc = CurMapPtr->GetPortalLoc();
				SetActorLocation(organizedLoc);

				Fsm.ChangeState(EPlayerState::PORTAL);
				AGameUI::StopTimer();
				return;
			}
		}
		else
		{
			FIntPoint nextIdxLT = CurMapPtr->LocationToMatrixIdx(nextPosLT);
			FIntPoint nextIdxRT = CurMapPtr->LocationToMatrixIdx(nextPosRT);
			FIntPoint nextIdxLB = CurMapPtr->LocationToMatrixIdx(nextPosLB);
			FIntPoint nextIdxRB = CurMapPtr->LocationToMatrixIdx(nextPosRB);

			if (nextIdxLT == portalIdx || nextIdxRT == portalIdx || nextIdxLB == portalIdx || nextIdxRB == portalIdx)
			{
				isBlockedByPortal = true;
				isMove = false;
			}
		}

		if (!isBlockedByPortal)
		{
			// TODO: set basis
			// Move with sliding
			if (Direction == FVector2D::UP || Direction == FVector2D::DOWN)
			{
				if (canMoveMapLT && !canMoveMapRT || canMoveMapLB && !canMoveMapRB)
				{
					AddActorLocation(FVector2D::LEFT * _deltaTime * RealSpeed);
					return;
				}
				else if (!canMoveMapLT && canMoveMapRT || !canMoveMapLB && canMoveMapRB)
				{
					AddActorLocation(FVector2D::RIGHT * _deltaTime * RealSpeed);
					return;
				}
			}
			else
			{
				if (canMoveMapRT && !canMoveMapRB || canMoveMapLT && !canMoveMapLB)
				{
					AddActorLocation(FVector2D::UP * _deltaTime * RealSpeed);
					return;
				}
				else if (!canMoveMapRT && canMoveMapRB || !canMoveMapLT && canMoveMapLB)
				{
					AddActorLocation(FVector2D::DOWN * _deltaTime * RealSpeed);
					return;
				}
			}
		}
	}

	if (isMove)
	{
		/*FVector2D nextPos = curLoc + Direction * _deltaTime * RealSpeed;
		FVector2D orginizedLoc = CurMapPtr->GetOrganizedLoc(nextPos);
		if (Direction == FVector2D::UP || Direction == FVector2D::DOWN)
		{
			nextPos.X = orginizedLoc.X;
		}
		else
		{
			nextPos.Y = orginizedLoc.Y;
		}
		
		OutputDebugString(("nextPos: " + std::to_string(nextPos.X) + ", " + std::to_string(nextPos.Y) + "\n").c_str());
		OutputDebugString(("orginizedLoc : " + std::to_string(orginizedLoc.X) + ", " + std::to_string(orginizedLoc.Y) + "\n").c_str());
		SetActorLocation(nextPos);*/

		AddActorLocation(Direction * _deltaTime * RealSpeed);
		UEngineSound::Play(SFXWalking, -1, 1);
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