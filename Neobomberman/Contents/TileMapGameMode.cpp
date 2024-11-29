#include "PreCompile.h"
#include "TileMapGameMode.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"

#include <EngineCore/Level.h>
#include <EnginePlatform/EngineInput.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineFile.h>
#include <EngineBase/EngineDirectory.h>

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
	if (EditorDatas.empty()) return;

	ATileMap* curMapPtr = EditorDatas[CurTilePtrIdx].Container;
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

void ATileMapGameMode::InitMouseRenderer()
{
	MouseSpriteRender = CreateDefaultSubObject<USpriteRenderer>();
	MouseSpriteRender->SetSprite(GlobalPath::TILE_STAGE_1_GUIDE, 0);
	MouseSpriteRender->SetComponentScale(GlobalVar::BOMB_SIZE);
	MouseSpriteRender->SetOrder(ERenderOrder::UI);
}

void ATileMapGameMode::InitMap()
{
	FIntPoint titleIdxs = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { titleIdxs.X * tileSize.X, titleIdxs.Y * tileSize.Y };
	FVector2D subSize = (winSize - mapSize) / 2;
	FIntPoint moveLoc{ static_cast<int>(subSize.X), static_cast<int>(subSize.Y) };

	{
		GroundTileMap = GetWorld()->SpawnActor<ATileMap>();
		GroundTileMap->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Ground);

		for (int y = 0; y < titleIdxs.Y; y++)
		{
			for (int x = 0; x < titleIdxs.X; x++)
			{
				// Stage1. temp
				//GroundTileMap->SetTile({ x, y }, 0, true);

				// Boss1. temp
				//GroundTileMap->SetTile({ x, y }, 1, true);

				// Battle. temp
				GroundTileMap->SetTile({ x, y }, 16, true);
			}
		}

		GroundTileMap->SetActorLocation(moveLoc);
	}

	{
		WallTileMap = GetWorld()->SpawnActor<ATileMap>();
		WallTileMap->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Wall);

		WallTileMap->SetActorLocation(moveLoc);
	}

	{
		BoxTileMap = GetWorld()->SpawnActor<ATileMap>();
		BoxTileMap->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Box);

		BoxTileMap->SetActorLocation(moveLoc);
	}

	{
		CoverTileMap = GetWorld()->SpawnActor<ATileMap>();
		CoverTileMap->Init(GlobalPath::TILE_STAGE_1, titleIdxs, tileSize, TileType::Cover);

		CoverTileMap->SetActorLocation(moveLoc);
	}

	// Init frame
	{
		// 241119 - number of files in Resources\Tiles\TileStage_1
		UEngineDirectory dir;
		dir.MoveRelative("Resources\\Tiles\\TileStage_1");	// Temp
		std::vector<UEngineFile> files = dir.GetAllFile(false);

		EditorDatas.reserve(files.size());
		for (size_t i = 0, size = files.size(); i < size; ++i)
		{
			EditorData data;

			// Temp
			if (i <= 16)
			{
				data.Type = TileType::Ground;
				data.Container = GroundTileMap;
			}
			else if (i <= 22)
			{
				data.Type = TileType::Wall;
				data.Container = WallTileMap;
			}
			else if (i <= 24)
			{
				data.Type = TileType::Box;
				data.Container = BoxTileMap;
			}
			else
			{
				data.Type = TileType::Cover;
				data.Container = CoverTileMap;
			}

			EditorDatas.push_back(data);
		}
	}
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
	CurTilePtrIdx = (CurTilePtrIdx + 1) % EditorDatas.size();
	UpdateMouseRenderer();
}

void ATileMapGameMode::AddTile(ATileMap* _curMapPtr, const FVector2D& _mousePos)
{
	TileType tileType = _curMapPtr->GetType();

	// Remove existing tile has same type
	for (int i = 0; i < EditorDatas.size(); i++)
	{
		if (tileType == EditorDatas[i].Type)
		{
			Tile* tile = EditorDatas[i].Container->GetTileRef(_mousePos);
			if (tile != nullptr && tile->SpriteRenderer != nullptr && tile->SpriteIndex != -1)
			{
				tile->SpriteRenderer->Destroy();
				tile->SpriteRenderer = nullptr;
				tile->SpriteIndex = -1;
			}
		}
	}

	bool isMove = true;
	if (tileType == TileType::Wall || tileType == TileType::Box)
	{
		isMove = false;
	}
	_curMapPtr->SetTileWithLoc(_mousePos, CurTilePtrIdx, isMove);
}

void ATileMapGameMode::RemoveTile(ATileMap* _curMapPtr, const FVector2D& _mousePos)
{
	Tile* tile = _curMapPtr->GetTileRef(_mousePos);
	if (tile != nullptr && tile->SpriteRenderer != nullptr)
	{
		tile->SpriteRenderer->Destroy();
		tile->SpriteRenderer = nullptr;
		tile->SpriteIndex = -1;
	}
}

void ATileMapGameMode::SaveTileMap()
{
	Serialize(GroundTileMap, TileDataPath, GlobalPath::MAP_GROUND_DAT);
	Serialize(WallTileMap, TileDataPath, GlobalPath::MAP_WALL_DAT);
	Serialize(BoxTileMap, TileDataPath, GlobalPath::MAP_BOX_DAT);
	Serialize(CoverTileMap, TileDataPath, GlobalPath::MAP_COVER_DAT);
	MessageBoxA(nullptr, "Saved", "Info", MB_OK);
}

void ATileMapGameMode::LoadTileMap()
{
	Deserialize(GroundTileMap, TileDataPath, GlobalPath::MAP_GROUND_DAT);
	Deserialize(WallTileMap, TileDataPath, GlobalPath::MAP_WALL_DAT);
	Deserialize(BoxTileMap, TileDataPath, GlobalPath::MAP_BOX_DAT);
	Deserialize(CoverTileMap, TileDataPath, GlobalPath::MAP_COVER_DAT);
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