#include "PreCompile.h"
#include "TileMapGameMode.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"

#include <EngineCore/Level.h>
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineFile.h>
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineRandom.h>

ATileMapGameMode::ATileMapGameMode()
{
	InitMouseRenderer();
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
		GroundTileMap->Init(TILE_IMG_FOLDER_NAME, titleIdxs, tileSize, TileType::Ground);

		for (int y = 0; y < titleIdxs.Y; y++)
		{
			for (int x = 0; x < titleIdxs.X; x++)
			{
				GroundTileMap->SetSpriteAndIndex({ x, y }, static_cast<int>(TileType::Ground));
			}
		}

		GroundTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(GroundTileMap);
	}

	{
		WallTileMap = GetWorld()->SpawnActor<ATileMap>();
		WallTileMap->Init(TILE_IMG_FOLDER_NAME, titleIdxs, tileSize, TileType::Wall);

		WallTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(WallTileMap);
	}

	{
		BoxTileMap = GetWorld()->SpawnActor<ATileMap>();
		BoxTileMap->Init(TILE_IMG_FOLDER_NAME, titleIdxs, tileSize, TileType::Box);

		BoxTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(BoxTileMap);
	}
}

void ATileMapGameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (TilePtrs.size() == 0) return;

	ATileMap* CurMapPtr = TilePtrs[CurTilePtrIdx];
	if (CurMapPtr == nullptr) return;

	FVector2D mousePos = UEngineAPICore::GetCore()->GetMainWindow().GetMousePos();

	MoveMouseRenderer(CurMapPtr, mousePos);

	if (UEngineInput::GetInst().IsDown('N') == true)
	{
		CurTilePtrIdx = (CurTilePtrIdx + 1) % TilePtrs.size();
		UpdateMouseRenderer();
	}
	else if (UEngineInput::GetInst().IsPress(VK_LBUTTON) == true)
	{
		// Temp: i=1, 0 is ground.
		for (int i = 1; i < TilePtrs.size(); i++)
		{
			ATileMap* otherMapPtr = TilePtrs[i];
			if (CurMapPtr != otherMapPtr)
			{
				Tile* tile = otherMapPtr->GetTileRef(mousePos);
				if (tile != nullptr && tile->SpriteRenderer != nullptr && tile->SpriteIndex != -1)
				{
					tile->SpriteRenderer->Destroy();
					tile->SpriteRenderer = nullptr;
				}
			}
		}

		CurMapPtr->SetSpriteAndIndexWithLocation(mousePos, static_cast<int>(CurMapPtr->GetType()));
	}
	else if (UEngineInput::GetInst().IsPress(VK_RBUTTON) == true)
	{
		Tile* tile = CurMapPtr->GetTileRef(mousePos);
		if (tile != nullptr && tile->SpriteRenderer != nullptr)
		{
			tile->SpriteRenderer->Destroy();
			tile->SpriteRenderer = nullptr;
		}
	}
	else if (UEngineInput::GetInst().IsPress('S') == true)
	{
		Serialize(WallTileMap, TILE_DAT_PATH, "WallData.dat");
		Serialize(BoxTileMap, TILE_DAT_PATH, "BoxData.dat");
		MessageBoxA(nullptr, "Saved", "Info", MB_OK);
	}

	if (UEngineInput::GetInst().IsPress('R') == true)
	{
		Deserialize(WallTileMap, TILE_DAT_PATH, "WallData.dat");
		Deserialize(BoxTileMap, TILE_DAT_PATH, "BoxData.dat");
	}
}

void ATileMapGameMode::InitMouseRenderer()
{
	MouseSpriteRender = CreateDefaultSubObject<USpriteRenderer>();
	MouseSpriteRender->SetSprite(TILE_GUIDE_IMG_FOLDER_NAME, 0);
	MouseSpriteRender->SetComponentScale(GlobalVar::BOMB_SIZE);
	MouseSpriteRender->SetOrder(ERenderOrder::UI);
}

void ATileMapGameMode::MoveMouseRenderer(ATileMap* _curMapPtr, const FVector2D& _mousePos)
{
	FIntPoint nowIdx = _curMapPtr->LocationToIndex(_mousePos);

	if (MouseIdx != nowIdx)
	{
		MouseIdx = nowIdx;
		FVector2D loc = _curMapPtr->IndexToLocation(MouseIdx);
		FVector2D tileSize = _curMapPtr->GetTileSize();
		loc += { tileSize.X / 2, tileSize.Y };
		MouseSpriteRender->SetComponentLocation(loc);
	}
}

void ATileMapGameMode::UpdateMouseRenderer()
{
	MouseSpriteRender->SetSprite(TILE_GUIDE_IMG_FOLDER_NAME, CurTilePtrIdx);
}

bool ATileMapGameMode::Serialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName)
{
	if (_tileMap == nullptr) return false;

	UEngineDirectory dir;
	dir.MoveRelative(_savePath);

	UEngineSerializer serializer;
	_tileMap->Serialize(serializer);

	UEngineFile file = dir.GetPathToString() + "\\" + _saveName.data();
	file.FileOpen("wb");
	file.Write(serializer);
	file.Close();

	return true;
}

bool ATileMapGameMode::Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName)
{
	if (_tileMap == nullptr) return false;

	UEngineDirectory dir;
	dir.MoveRelative(_savePath);

	UEngineSerializer serializer;
	UEngineFile file = dir.GetPathToString() + "\\" + _saveName.data();
	file.FileOpen("rb");
	file.Read(serializer);
	_tileMap->DeSerialize(serializer);
	file.Close();

	return true;
}