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

	GlobalPath path;
	TileDataPath = path.GetTileDataPath();
}

ATileMapGameMode::~ATileMapGameMode()
{
}

void ATileMapGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitMap();
}

void ATileMapGameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	/** checking **/
	if (TilePtrs.size() == 0) return;

	ATileMap* curMapPtr = TilePtrs[CurTilePtrIdx];
	if (curMapPtr == nullptr) return;

	/** logic **/
	FVector2D mousePos = UEngineAPICore::GetCore()->GetMainWindow().GetMousePos();
	MoveMouseRenderer(curMapPtr, mousePos);
	
	if (UEngineInput::GetInst().IsDown('N') == true)
	{
		NextTile();
	}
	else if (UEngineInput::GetInst().IsPress(VK_LBUTTON) == true)
	{
		AddTile(curMapPtr, mousePos);
	}
	else if (UEngineInput::GetInst().IsPress(VK_RBUTTON) == true)
	{
		RemoveTile(curMapPtr, mousePos);
	}
	else if (UEngineInput::GetInst().IsPress('S') == true)
	{
		SaveTileMap();
	}

	if (UEngineInput::GetInst().IsPress('R') == true)
	{
		LoadTileMap();
	}
}

void ATileMapGameMode::InitMap()
{
	FIntPoint titleIdxs = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { titleIdxs.X * tileSize.X, titleIdxs.Y * tileSize.Y };
	FVector2D subSize = (winSize - mapSize) / 2;
	FIntPoint moveLoc{ static_cast<int>(subSize.X), static_cast<int>(subSize.Y) };

	TilePtrs.reserve(10);	// Temp. TODO: img count in directory

	{
		GroundTileMap = GetWorld()->SpawnActor<ATileMap>();
		GroundTileMap->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Ground);

		for (int y = 0; y < titleIdxs.Y; y++)
		{
			for (int x = 0; x < titleIdxs.X; x++)
			{
				GroundTileMap->SetTile({ x, y }, static_cast<int>(TileType::Ground), true);
			}
		}

		GroundTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(GroundTileMap);
	}

	{
		WallTileMap = GetWorld()->SpawnActor<ATileMap>();
		WallTileMap->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Wall);

		WallTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(WallTileMap);
	}

	{
		BoxTileMap = GetWorld()->SpawnActor<ATileMap>();
		BoxTileMap->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Box);

		BoxTileMap->SetActorLocation(moveLoc);
		TilePtrs.push_back(BoxTileMap);
	}
}

void ATileMapGameMode::InitMouseRenderer()
{
	MouseSpriteRender = CreateDefaultSubObject<USpriteRenderer>();
	MouseSpriteRender->SetSprite(GlobalPath::TILE_STAGE_1_GUIDE, 0);
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
	MouseSpriteRender->SetSprite(GlobalPath::TILE_STAGE_1_GUIDE, CurTilePtrIdx);
}

void ATileMapGameMode::NextTile()
{
	CurTilePtrIdx = (CurTilePtrIdx + 1) % TilePtrs.size();
	UpdateMouseRenderer();
}

void ATileMapGameMode::AddTile(ATileMap* _curMapPtr, const FVector2D& _mousePos)
{
	// Remove existing tile
	// Temp: i=1, 0 is ground.
	for (int i = 1; i < TilePtrs.size(); i++)
	{
		ATileMap* otherMapPtr = TilePtrs[i];
		if (_curMapPtr != otherMapPtr)
		{
			Tile* tile = otherMapPtr->GetTileRef(_mousePos);
			if (tile != nullptr && tile->SpriteRenderer != nullptr && tile->SpriteIndex != -1)
			{
				tile->SpriteRenderer->Destroy();
				tile->SpriteRenderer = nullptr;
			}
		}
	}

	bool isMove = true;
	if (_curMapPtr->GetType() != TileType::Ground)
	{
		isMove = false;
	}
	_curMapPtr->SetTileWithLoc(_mousePos, static_cast<int>(_curMapPtr->GetType()), isMove);
}

void ATileMapGameMode::RemoveTile(ATileMap* _curMapPtr, const FVector2D& _mousePos)
{
	Tile* tile = _curMapPtr->GetTileRef(_mousePos);
	if (tile != nullptr && tile->SpriteRenderer != nullptr)
	{
		tile->SpriteRenderer->Destroy();
		tile->SpriteRenderer = nullptr;
	}
}

void ATileMapGameMode::SaveTileMap()
{
	Serialize(WallTileMap, TileDataPath, GlobalPath::MAP_WALL_DAT);
	Serialize(BoxTileMap, TileDataPath, GlobalPath::MAP_BOX_DAT);
	MessageBoxA(nullptr, "Saved", "Info", MB_OK);
}

void ATileMapGameMode::LoadTileMap()
{
	Deserialize(WallTileMap, TileDataPath, GlobalPath::MAP_WALL_DAT);
	Deserialize(BoxTileMap, TileDataPath, GlobalPath::MAP_BOX_DAT);
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