#include "PreCompile.h"
#include "ContentsEnum.h"
#include "TileMap.h"
#include "PlayMap.h"
#include "Player.h"
#include "BombManager.h"

#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineCoreDebug.h>

#include <EnginePlatform/EngineInput.h>
#include <EngineCore/ImageManager.h>
#include "GlobalVar.h"

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

		SpriteRendererHead->CreateAnimation("Idle_Up_Head", PLAYER_SPRITE_PATH, 17, 17, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Up_Head", PLAYER_SPRITE_PATH, 18, 22, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Down_Head", PLAYER_SPRITE_PATH, 0, 0, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Down_Head", PLAYER_SPRITE_PATH, 1, 6, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Left_Head", PLAYER_SPRITE_PATH, 9, 9, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Left_Head", PLAYER_SPRITE_PATH, 10, 14, 0.1f);

		SpriteRendererHead->CreateAnimation("Idle_Right_Head", PLAYER_SPRITE_PATH, 24, 24, 0.1f);
		SpriteRendererHead->CreateAnimation("Run_Right_Head", PLAYER_SPRITE_PATH, 25, 30, 0.1f);
	}

	{
		SpriteRendererBody = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererBody->SetSprite(PLAYER_SPRITE_PATH);
		SpriteRendererBody->SetComponentLocation({ 0, static_cast<int>(playerSize.Y *.5f)});
		SpriteRendererBody->SetComponentScale(playerSize);
		SpriteRendererBody->SetPivotType(PivotType::Bot);

		SpriteRendererBody->CreateAnimation("Idle_Up_Body", PLAYER_SPRITE_PATH, 49, 49, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Up_Body", PLAYER_SPRITE_PATH, 50, 54, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Down_Body", PLAYER_SPRITE_PATH, 33, 33, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Down_Body", PLAYER_SPRITE_PATH, 34, 38, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Left_Body", PLAYER_SPRITE_PATH, 41, 41, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Left_Body", PLAYER_SPRITE_PATH, 42, 46, 0.1f);

		SpriteRendererBody->CreateAnimation("Idle_Right_Body", PLAYER_SPRITE_PATH, 57, 57, 0.1f);
		SpriteRendererBody->CreateAnimation("Run_Right_Body", PLAYER_SPRITE_PATH, 58, 62, 0.1f);
	}

	SpriteRendererHead->ChangeAnimation("Idle_Down_Head");
	SpriteRendererBody->ChangeAnimation("Idle_Down_Body");

	SpriteRendererHead->SetOrder(ERenderOrder::PLAYER);
	SpriteRendererBody->SetOrder(ERenderOrder::PLAYER);
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	Super::BeginPlay();

	BombManager = GetWorld()->SpawnActor<ABombManager>();
	BombManager->Init(GlobalVar::BATTLE_GROUND_COUNT, GlobalVar::BOMB_SIZE);

	// Set camera
	//	FVector2D Size = UEngineAPICore::GetCore()->GetMainWindow().GetWindowSize();
	//	GetWorld()->SetCameraPivot(Size.Half() * -1.0f);
}

void APlayer::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	//UEngineDebug::CoreOutPutString("FPS : " + std::to_string(1.0f / _DeltaTime));
	//UEngineDebug::CoreOutPutString("PlayerPos : " + GetActorLocation().ToString());

	bool isPressedD = UEngineInput::GetInst().IsPress('D');
	bool isPressedA = UEngineInput::GetInst().IsPress('A');
	bool isPressedS = UEngineInput::GetInst().IsPress('S');
	bool isPressedW = UEngineInput::GetInst().IsPress('W');
	bool isDownSpace = UEngineInput::GetInst().IsDown(VK_SPACE);
	FVector2D direction;
	int tempDir = 0;

	if (isPressedD)
	{
		SpriteRendererHead->ChangeAnimation("Run_Right_Head");
		SpriteRendererBody->ChangeAnimation("Run_Right_Body");
		direction = FVector2D::RIGHT;
		tempDir = 1;
	}
	else if (isPressedA)
	{
		SpriteRendererHead->ChangeAnimation("Run_Left_Head");
		SpriteRendererBody->ChangeAnimation("Run_Left_Body");
		direction = FVector2D::LEFT;
		tempDir = 2;
	}
	else if (isPressedS)
	{
		SpriteRendererHead->ChangeAnimation("Run_Down_Head");
		SpriteRendererBody->ChangeAnimation("Run_Down_Body");
		direction = FVector2D::DOWN;
		tempDir = 3;
	}
	else if (isPressedW)
	{
		SpriteRendererHead->ChangeAnimation("Run_Up_Head");
		SpriteRendererBody->ChangeAnimation("Run_Up_Body");
		direction = FVector2D::UP;
		tempDir = 4;
	}
	else if (!isPressedD && !isPressedA && !isPressedS && !isPressedW)
	{
		if (UEngineInput::GetInst().IsUp('D'))
		{
			SpriteRendererHead->ChangeAnimation("Idle_Right_Head");
			SpriteRendererBody->ChangeAnimation("Idle_Right_Body");
		}
		else if (UEngineInput::GetInst().IsUp('A'))
		{
			SpriteRendererHead->ChangeAnimation("Idle_Left_Head");
			SpriteRendererBody->ChangeAnimation("Idle_Left_Body");
		}
		else if (UEngineInput::GetInst().IsUp('S'))
		{
			SpriteRendererHead->ChangeAnimation("Idle_Down_Head");
			SpriteRendererBody->ChangeAnimation("Idle_Down_Body");
		}
		else if (UEngineInput::GetInst().IsUp('W'))
		{
			SpriteRendererHead->ChangeAnimation("Idle_Up_Head");
			SpriteRendererBody->ChangeAnimation("Idle_Up_Body");
		}
		direction = FVector2D::ZERO;
	}

	bool isMove = false;

	// TODO: tempDir
	FVector2D extPos = direction;
	switch (tempDir)
	{
		case 1: extPos *= 16.f; break;
		case 2: extPos *= 16.f; break;
		case 3: extPos *= 9.f; break;
		case 4: extPos *= 9.f; break;
		default: break;
	}
	FVector2D nextPos = GetActorLocation() + extPos + (direction * _DeltaTime * Speed);

	// Temp
	const int POS_X_MIN = 96;	// 48
	const int POS_X_MAX = 512;	// 256
	const int POS_Y_MIN = 64 + 12;	// 32 + 8
	const int POS_Y_MAX = 416;	// 208 - 4;

	if (nextPos.X >= POS_X_MIN && nextPos.X < POS_X_MAX && nextPos.Y >= POS_Y_MIN && nextPos.Y < POS_Y_MAX)
	{
		isMove = true;
	}

	//if (CollisionImage != nullptr)
	//{
	//	UColor color = CollisionImage->GetColor(nextPos, UColor::BLACK);
	//	if (color == UColor::WHITE)
	//	{
	//		isMove = true;
	//	}
	//}

	//FVector2D nowPos = GetActorLocation();
	//std::string nowPosStr = (std::to_string(nowPos.X) + ", " + std::to_string(nowPos.Y));
	//std::string nextPosStr = (std::to_string(nextPos.X) + ", " + std::to_string(nextPos.Y) + "\n");
	//OutputDebugString((nowPosStr + " -> " + nextPosStr).c_str());

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
		AddActorLocation(direction * _DeltaTime * Speed);
	}
	
	if (isDownSpace)
	{
		BombManager->SetBomb(GetActorLocation(), Ability.BombType, Ability.Power);
	}
}

void APlayer::RunSoundPlay()
{
}

void APlayer::LevelChangeStart()
{
	Super::LevelChangeStart();
}

void APlayer::LevelChangeEnd()
{
	Super::LevelChangeEnd();
}

void APlayer::SetCollisionImage(std::string_view _ColImageName)
{
	CollisionImage = UImageManager::GetInst().FindImage(_ColImageName);
}

void APlayer::SetCurMap(APlayMap* _map)
{
	CurMap = _map;
}