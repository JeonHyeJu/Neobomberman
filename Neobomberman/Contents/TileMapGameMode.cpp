#include "PreCompile.h"
#include "TileMapGameMode.h"
#include "GlobalVar.h"

#include <EngineCore/Level.h>
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineFile.h>
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineRandom.h>

ATileMapGameMode::ATileMapGameMode()
{
}

ATileMapGameMode::~ATileMapGameMode()
{
}

void ATileMapGameMode::BeginPlay()
{
	Super::BeginPlay();

	FIntPoint titleIdxs = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { titleIdxs.X * tileSize.X, titleIdxs.Y * tileSize.Y };
	FVector2D subSize = (winSize - mapSize) / 2;
	FIntPoint moveLoc{ static_cast<int>(subSize.X), static_cast<int>(subSize.Y) };

	TilePtrs.reserve(10);	// Temp. TODO: img count in directory

	{
		GroundTileMap = GetWorld()->SpawnActor<ATileMap>();
		GroundTileMap->Init("TileStage_1", titleIdxs, tileSize, TileType::Ground);

		for (int y = 0; y < titleIdxs.Y; y++)
		{
			for (int x = 0; x < titleIdxs.X; x++)
			{
				GroundTileMap->SetSpriteAndIndex({ x, y }, 0);
			}
		}
		
		GroundTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(GroundTileMap);
	}

	{
		BoxTileMap = GetWorld()->SpawnActor<ATileMap>();
		BoxTileMap->Init("TileStage_1", titleIdxs, tileSize, TileType::Box);

		//for (int y = 0; y < titleIdxs.Y; y++)
		//{
		//	for (int x = 0; x < titleIdxs.X; x++)
		//	{
		//		BoxTileMap->SetSpriteAndIndex({ x, y }, 1);
		//	}
		//}

		BoxTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(BoxTileMap);
	}

	{
		WallTileMap = GetWorld()->SpawnActor<ATileMap>();
		WallTileMap->Init("TileStage_1", titleIdxs, tileSize, TileType::Wall);

		//for (int y = 0; y < titleIdxs.Y; y++)
		//{
		//	for (int x = 0; x < titleIdxs.X; x++)
		//	{
		//		WallTileMap->SetSpriteAndIndex({ x, y }, 2);
		//	}
		//}

		WallTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(WallTileMap);
	}
}

void ATileMapGameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (true == UEngineInput::GetInst().IsPress(VK_LBUTTON))
	{
		FVector2D MousePos = UEngineAPICore::GetCore()->GetMainWindow().GetMousePos();
		ATileMap* CurMapPtr = TilePtrs[CurTilePtrIdx];
		CurMapPtr->SetSpriteAndIndexWithLocation(MousePos, static_cast<int>(CurMapPtr->GetType()));
	}

	if (true == UEngineInput::GetInst().IsPress(VK_RBUTTON))
	{
		FVector2D MousePos = UEngineAPICore::GetCore()->GetMainWindow().GetMousePos();
		ATileMap* CurMapPtr = TilePtrs[CurTilePtrIdx];
		Tile* tile = CurMapPtr->GetTileRef(MousePos);
		if (tile != nullptr)
		{
			if (tile->SpriteRenderer != nullptr)
			{
				int a = 0;
			}

			int a = 0;
		}
		
		if (tile != nullptr && tile->SpriteRenderer != nullptr)
		{
			tile->SpriteRenderer->Destroy();
			tile->SpriteRenderer = nullptr;
		}
	}

	if (true == UEngineInput::GetInst().IsPress('N'))
	{
		CurTilePtrIdx = (CurTilePtrIdx + 1) % TilePtrs.size();
	}

	//if (true == UEngineInput::GetInst().IsPress('R'))
	//{     
	//	UEngineSerializer _Ser;
	//	WallTileMap->Serialize(_Ser);

	//	UEngineDirectory Dir;

	//	if (false == Dir.MoveParentToDirectory("Resources"))
	//	{
	//		MSGASSERT("리소스 폴더를 찾지 못했습니다.");
	//		return;
	//	}

	//	Dir.Append("Data");

	//	std::string SaveFilePath = Dir.GetPathToString() + "\\MapData.Data";
	//	UEngineFile NewFile = SaveFilePath;
	//	NewFile.FileOpen("wb");
	//	NewFile.Write(_Ser);
	//}

	//if (true == UEngineInput::GetInst().IsPress('P'))
	//{
	//	UEngineRandom Random;
	//	for (size_t i = 0; i < 10; i++)
	//	{
	//		int Value = Random.RandomInt(0, 100);
	//		UEngineDebug::OutPutString(std::to_string(Value));
	//	}
	//}

	//if (true == UEngineInput::GetInst().IsPress('T'))
	//{
	//	UEngineDirectory Dir;

	//	if (false == Dir.MoveParentToDirectory("Resources"))
	//	{
	//		MSGASSERT("리소스 폴더를 찾지 못했습니다.");
	//		return;
	//	}

	//	Dir.Append("Data");

	//	std::string SaveFilePath = Dir.GetPathToString() + "\\MapData.Data";
	//	UEngineFile NewFile = SaveFilePath;
	//	NewFile.FileOpen("rb");

	//	UEngineSerializer Ser;
	//	NewFile.Read(Ser);


	//	WallTileMap->DeSerialize(Ser);

	//}
}
