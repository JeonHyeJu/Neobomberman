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

#include <EnginePlatform/EngineInput.h>

APlayer::APlayer()
{
	FVector2D playerSize = GlobalVar::BOMBERMAN_SIZE;

	{
		SpriteRendererHead = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererHead->SetSprite(PLAYER_SPRITE_PATH);
		SpriteRendererHead->SetComponentLocation(playerSize.Half().Half() + FVector2D{ -1.f, 6.f });
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

		std::vector<int> idxs{ 580, 581, 580, 581, 580 };
		idxs.resize(5);
		SpriteRendererHead->CreateAnimation("BlinkingEyes", PLAYER_SPRITE_PATH, idxs, 0.3f);
		SpriteRendererHead->CreateAnimation("Dead", PLAYER_SPRITE_PATH, 591, 600, 0.2f, false);
		SpriteRendererHead->CreateAnimation("Ride_Portal", PLAYER_SPRITE_PATH, 583, 589, 0.1f, false);
		SpriteRendererHead->SetAnimationEvent("Ride_Portal", 589, std::bind(&APlayer::OnEndPortalAnim, this));
	}
	
	{
		SpriteRendererBody = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererBody->SetSprite(PLAYER_SPRITE_PATH);
		SpriteRendererBody->SetComponentLocation(SpriteRendererHead->GetComponentLocation() + FVector2D{ 0.f, playerSize.hY() });
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
		SpriteRendererBody->CreateAnimation("Ride_Portal", PLAYER_SPRITE_PATH, 615, 621, 0.1f, false);
	}

	{
		FVector2D collSize = GlobalVar::BOMB_SIZE;

		Collision = CreateDefaultSubObject<U2DCollision>();
		Collision->SetComponentLocation(collSize.Half());
		Collision->SetComponentScale(collSize);
		Collision->SetCollisionGroup(ECollisionGroup::PlayerBody);
		Collision->SetCollisionType(ECollisionType::CirCle);

		Collision->SetCollisionEnter(std::bind(&APlayer::OnEnterCollision, this, std::placeholders::_1));

		GetWorld()->CollisionGroupLink(ECollisionGroup::PlayerBody, ECollisionGroup::MonsterBody);

		/*Collision->SetCollisionStay(std::bind(&ANewPlayer::CollisionStay, this, std::placeholders::_1));
		Collision->SetCollisionEnd(std::bind(&ANewPlayer::CollisionEnd, this, std::placeholders::_1));*/
	}

	DyingAnimInfo.AnimSeconds = 1.5f;
	DyingAnimInfo.WaitSeconds = 3.0f;

	SpriteRendererHead->SetOrder(ERenderOrder::PLAYER);
	SpriteRendererBody->SetOrder(ERenderOrder::PLAYER);

	FsmH.CreateState(EPlayerState::REBORN, std::bind(&APlayer::Reborning, this, std::placeholders::_1), std::bind(&APlayer::OnReborn, this));
	FsmH.CreateState(EPlayerState::IDLE, std::bind(&APlayer::Idling, this, std::placeholders::_1), std::bind(&APlayer::OnIdle, this));
	FsmH.CreateState(EPlayerState::MOVE, std::bind(&APlayer::Moving, this, std::placeholders::_1));
	FsmH.CreateState(EPlayerState::DEAD, std::bind(&APlayer::Dying, this, std::placeholders::_1), std::bind(&APlayer::OnDead, this));
	FsmH.CreateState(EPlayerState::PORTAL, nullptr, std::bind(&APlayer::OnShift, this));

	DebugOn();
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

	// state != DEAD and PORTAL
	if (nowState < EPlayerState::DEAD)
	{
		bool isDownSpace = UEngineInput::GetInst().IsDown(VK_SPACE);
		if (isDownSpace)
		{
			// Temp
			//std::map<std::string, USoundPlayer>::iterator iterSfx = Sounds.find("DropBomb");
			//if (iterSfx != Sounds.end())
			//{
			//	iterSfx->second.On();
			//}
			USoundPlayer splayer = UEngineSound::Play("CreateBomb.mp3");
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

void APlayer::LevelChangeStart()
{
	IsClear = false;
	IsDead = false;
}

void APlayer::InitSounds()
{
	// TODO: Do I need to change string to enum?
	//USoundPlayer splayer = UEngineSound::Play("CreateBomb.mp3");
	//Sounds.insert({"DropBomb", splayer});
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
		ABomb* pBomb = GetWorld()->SpawnActor<ABomb>();
		pBomb->Init(loc, Ability.BombType, Ability.Power, CurMapPtr);
	}
}

void APlayer::Kill()
{
	FsmH.ChangeState(EPlayerState::DEAD);
}

void APlayer::OnEnterCollision(AActor* _actor)
{
	//_actor->SetActive(false);	// temp
	Kill();
}

void APlayer::OnEndPortalAnim()
{
	IsClear = true;
}

/* FSM start functions */
void APlayer::OnReborn()
{
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

void APlayer::OnDead()
{
	GameData::GetInstance().AddPlayer1Life(-1);
	GameData::GetInstance().ResetScore();
	SpriteRendererHead->ChangeAnimation("Dead");
	SpriteRendererBody->ChangeAnimation("Dead");
}

void APlayer::OnShift()
{
	SpriteRendererHead->ChangeAnimation("Ride_Portal");
	SpriteRendererBody->ChangeAnimation("Ride_Portal");
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

	FVector2D curLoc = GetActorLocation();
	FVector2D nextPosLT = curLoc + (Direction * _deltaTime * Ability.Speed) + FVector2D{ 1, 1 };
	FVector2D nextPosRT = curLoc + (Direction * _deltaTime * Ability.Speed) + FVector2D{ 31, 1 };
	FVector2D nextPosLB = curLoc + (Direction * _deltaTime * Ability.Speed) + FVector2D{ 1, 31 };
	FVector2D nextPosRB = curLoc + (Direction * _deltaTime * Ability.Speed) + FVector2D{ 31, 31 };

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
		bool canMoveMapLT = CurMapPtr->CanMove(nextPosLT);
		bool canMoveMapRT = CurMapPtr->CanMove(nextPosRT);
		bool canMoveMapLB = CurMapPtr->CanMove(nextPosLB);
		bool canMoveMapRB = CurMapPtr->CanMove(nextPosRB);
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

				FsmH.ChangeState(EPlayerState::PORTAL);
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
					AddActorLocation(FVector2D::LEFT * _deltaTime * Ability.Speed);
					return;
				}
				else if (!canMoveMapLT && canMoveMapRT || !canMoveMapLB && canMoveMapRB)
				{
					AddActorLocation(FVector2D::RIGHT * _deltaTime * Ability.Speed);
					return;
				}
			}
			else
			{
				if (canMoveMapRT && !canMoveMapRB || canMoveMapLT && !canMoveMapLB)
				{
					AddActorLocation(FVector2D::UP * _deltaTime * Ability.Speed);
					return;
				}
				else if (!canMoveMapRT && canMoveMapRB || !canMoveMapLT && canMoveMapLB)
				{
					AddActorLocation(FVector2D::DOWN * _deltaTime * Ability.Speed);
					return;
				}
			}
		}
	}

	if (isMove)
	{
		/*FVector2D nextPos = curLoc + Direction * _deltaTime * Ability.Speed;
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
			IsDead = true;

			// only once
			if (!isOff)
			{
				isOff = true;
				SpriteRendererHead->SetActive(false);
				SpriteRendererBody->SetActive(false);
			}

			if (GameData::GetInstance().GetPlayer1Life() > 0)
			{
				if (DyingAnimInfo.Seconds >= DyingAnimInfo.WaitSeconds)
				{
					isOff = false;
					elapsedSecs = 0.f;
					DyingAnimInfo.Seconds = 0.f;

					FsmH.ChangeState(EPlayerState::REBORN);
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