#include "PreCompile.h"
#include "Player.h"
#include "BombPlain.h"

#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/EngineCoreDebug.h>

#include <EnginePlatform/EngineInput.h>
#include <EngineCore/ImageManager.h>
#include "GlobalVar.h"

APlayer::APlayer()
{
	FVector2D playerSize = GlobalVar::BOMBERMAN_SIZE;

	SetActorLocation({100, 100});

	{
		SpriteRendererHead = CreateDefaultSubObject<USpriteRenderer>();
		SpriteRendererHead->SetSprite(PLAYER_SPRITE_PATH);
		SpriteRendererHead->SetComponentLocation({ 0, 0 });
		SpriteRendererHead->SetComponentScale({ 32, 32 });

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
		SpriteRendererBody->SetComponentLocation({ 0, 16 });
		SpriteRendererBody->SetComponentScale({ 32, 32 });

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
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	Super::BeginPlay();

	// Set camera
	//	FVector2D Size = UEngineAPICore::GetCore()->GetMainWindow().GetWindowSize();
	//	GetWorld()->SetCameraPivot(Size.Half() * -1.0f);
}

void APlayer::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	UEngineDebug::CoreOutPutString("FPS : " + std::to_string(1.0f / _DeltaTime));
	UEngineDebug::CoreOutPutString("PlayerPos : " + GetActorLocation().ToString());

	bool isPressedD = UEngineInput::GetInst().IsPress('D');
	bool isPressedA = UEngineInput::GetInst().IsPress('A');
	bool isPressedS = UEngineInput::GetInst().IsPress('S');
	bool isPressedW = UEngineInput::GetInst().IsPress('W');
	bool isDownSpace = UEngineInput::GetInst().IsDown(VK_SPACE);

	if (isPressedD)
	{
		SpriteRendererHead->ChangeAnimation("Run_Right_Head");
		SpriteRendererBody->ChangeAnimation("Run_Right_Body");
		AddActorLocation(FVector2D::RIGHT * _DeltaTime * Speed);
	}
	else if (isPressedA)
	{
		SpriteRendererHead->ChangeAnimation("Run_Left_Head");
		SpriteRendererBody->ChangeAnimation("Run_Left_Body");
		AddActorLocation(FVector2D::LEFT * _DeltaTime * Speed);
	}
	else if (isPressedS)
	{
		SpriteRendererHead->ChangeAnimation("Run_Down_Head");
		SpriteRendererBody->ChangeAnimation("Run_Down_Body");
		AddActorLocation(FVector2D::DOWN * _DeltaTime * Speed);
	}
	else if (isPressedW)
	{
		SpriteRendererHead->ChangeAnimation("Run_Up_Head");
		SpriteRendererBody->ChangeAnimation("Run_Up_Body");
		AddActorLocation(FVector2D::UP * _DeltaTime * Speed);
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
	}

	if (isDownSpace)
	{
		BombPlain* pBomb = GetWorld()->SpawnActor<BombPlain>();
		pBomb->SetActorLocation(GetActorLocation());
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