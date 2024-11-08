#include "PreCompile.h"
#include "PlayMap.h"
#include "TileMap.h"
#include "Bomb.h"

#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/EngineAPICore.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/ImageManager.h>

void _ReorginizeExplosion(std::vector<EBombTailType>& _bombTrails)
{
	for (size_t i = 0, size = _bombTrails.size(); i < size - 1; ++i)
	{
		if (i + 1 >= size) break;

		if (_bombTrails[i] == EBombTailType::END && _bombTrails[i + 1] == EBombTailType::END)
		{
			_bombTrails[i] = EBombTailType::CONNECT;
		}
		else if (_bombTrails[i] == EBombTailType::END && _bombTrails[i + 1] == EBombTailType::CONNECT)
		{
			_bombTrails[i] = EBombTailType::CONNECT;
		}
	}
}

APlayMap::APlayMap()
{
	USpriteRenderer* OuterLineRenderer = CreateDefaultSubObject<USpriteRenderer>();
	OuterLineRenderer->SetOrder(ERenderOrder::COLMAP);
	OuterLineRenderer->SetSprite("Bg_1-Col.png");

	FVector2D MapScale = OuterLineRenderer->SetSpriteScale(1.0f);
	OuterLineRenderer->SetSpriteScale(1.0f);
	OuterLineRenderer->SetComponentLocation(MapScale.Half());
	OuterLineRenderer->SetActive(false);
}

APlayMap::~APlayMap()
{
}

void APlayMap::BeginPlay()
{
	Super::BeginPlay();

	InitMap();
	BombList.clear();
}

void APlayMap::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	CheckExplodedBomb();
	CheckExplodedBox();
}

void APlayMap::InitMap()
{
	FIntPoint tileIdxs = GlobalVar::BATTLE_GROUND_COUNT;
	FVector2D tileSize = GlobalVar::BOMB_SIZE;

	FVector2D winSize = GlobalVar::WINDOW_SIZE;
	FVector2D mapSize = { tileIdxs.X * tileSize.X, tileIdxs.Y * tileSize.Y };
	FVector2D subSize = winSize - mapSize;
	FIntPoint moveLoc{ static_cast<int>(subSize.X / 2), GlobalVar::STAGE_H_MARGIN };

	MapGround = GetWorld()->SpawnActor<ATileMap>();
	MapGround->Init(GlobalPath::TILE_STAGE_1, tileIdxs, tileSize, TileType::Ground);
	for (int y = 0; y < tileIdxs.Y; y++)
	{
		for (int x = 0; x < tileIdxs.X; x++)
		{
			MapGround->SetTile({ x, y }, static_cast<int>(TileType::Ground), true);
		}
	}
	MapGround->SetActorLocation(moveLoc);

	MapWall = GetWorld()->SpawnActor<ATileMap>();
	MapWall->Init(GlobalPath::TILE_STAGE_1, tileIdxs, tileSize, TileType::Wall);
	MapWall->SetActorLocation(moveLoc);

	MapBox = GetWorld()->SpawnActor<ATileMap>();
	MapBox->Init(GlobalPath::TILE_STAGE_1, tileIdxs, tileSize, TileType::Box);
	MapBox->SetActorLocation(moveLoc);

	GlobalPath path;
	std::string tileDatPath = path.GetTileDataPath();

	Deserialize(MapWall, tileDatPath, GlobalPath::MAP_WALL_DAT);
	Deserialize(MapBox, tileDatPath, GlobalPath::MAP_BOX_DAT);

	MapBox->SetTilesAnimAfterLoad(GlobalPath::ANIM_CRUMBLING_BOX, GlobalPath::ANIM_CRUMBLING_BOX);
}

void APlayMap::SetBomb(const FVector2D& _loc, EBombType _bombType, int _power)
{
	FIntPoint matIdx = MapGround->LocationToMatrixIdx(_loc);

	FIntPoint realIdx = MapGround->LocationToIndex(_loc);
	FVector2D orderedLoc = MapGround->IndexToLocation(realIdx);
	SBombTailTypes bombTailTypes = GetBombTailTypes(matIdx, _bombType, _power);

	std::list<ABomb*>::iterator it = BombList.begin();
	std::list<ABomb*>::iterator itEnd = BombList.end();

	for (; it != itEnd; ++it)
	{
		if (matIdx == (*it)->MatrixIdx)
		{
			// If a bomb already exists, don't make it.
			return;
		}
	}

	std::vector<FIntPoint> explodeIdxs = GetBombRange(matIdx, bombTailTypes);

	ABomb* pBomb = GetWorld()->SpawnActor<ABomb>();
	pBomb->SetActorLocation(orderedLoc);
	pBomb->Size = GlobalVar::BOMB_SIZE;
	pBomb->Power = _power;
	pBomb->BombType = _bombType;
	pBomb->MatrixIdx = matIdx;
	pBomb->ExplodeIdxs = explodeIdxs;
	pBomb->InitSpriteAndAnim(bombTailTypes);

	BombList.push_back(pBomb);
}

SBombTailTypes APlayMap::GetBombTailTypes(const FIntPoint& _matIdx, EBombType _bombType, int _power)
{
	SBombTailTypes bombTailTypes;

	bool isUpEnd = false;
	bool isDownEnd = false;
	bool isLeftEnd = false;
	bool isRightEnd = false;

	for (int i = 1; i <= _power; i++)
	{
		EBombTailType upType = GetBombTailType(_matIdx + FIntPoint::UP * i, &isUpEnd);
		EBombTailType downType = GetBombTailType(_matIdx + FIntPoint::DOWN * i, &isDownEnd);
		EBombTailType leftType = GetBombTailType(_matIdx + FIntPoint::LEFT * i, &isLeftEnd);
		EBombTailType rightType = GetBombTailType(_matIdx + FIntPoint::RIGHT * i, &isRightEnd);

		if (upType != EBombTailType::NONE) bombTailTypes.Up.push_back(upType);
		if (downType != EBombTailType::NONE) bombTailTypes.Down.push_back(downType);
		if (leftType != EBombTailType::NONE) bombTailTypes.Left.push_back(leftType);
		if (rightType != EBombTailType::NONE) bombTailTypes.Right.push_back(rightType);
	}

	_ReorginizeExplosion(bombTailTypes.Up);
	_ReorginizeExplosion(bombTailTypes.Down);
	_ReorginizeExplosion(bombTailTypes.Left);
	_ReorginizeExplosion(bombTailTypes.Right);

	return bombTailTypes;
}

EBombTailType APlayMap::GetBombTailType(const FIntPoint& _nextIdx, bool* isEnd)
{
	bool hasWall = MapWall->IsBlocked(_nextIdx);
	bool hasBox = MapBox->IsBlocked(_nextIdx);

	if (hasWall == false && *isEnd == false)
	{
		if (hasBox == true)
		{
			*isEnd = true;
			return EBombTailType::CONNECT;
		}
		else
		{
			return EBombTailType::END;
		}
	}
	else
	{
		*isEnd = true;
	}
	return EBombTailType::NONE;
}

bool APlayMap::Deserialize(ATileMap* _tileMap, std::string_view _savePath, std::string_view _saveName)
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

void APlayMap::CheckExplodedBomb()
{
	std::list<ABomb*>::iterator it = BombList.begin();
	std::list<ABomb*>::iterator itEnd = BombList.end();

	for (; it != itEnd; ++it)
	{
		ABomb* pBomb = *it;
		if (pBomb == nullptr) continue;

		if (pBomb->State == BombState::Exploding)
		{
			pBomb->SetState(BombState::FinishExploding);

			// Temp.. TODO!!
			/*for (size_t i = 0, size = pBomb->ExplodeIdxs.size(); i < size; ++i)
			{
				if (pBomb->ExplodeIdxs[i] == pBomb->MatrixIdx)
				{
					pBomb->ExplodeIntermediatly();
				}
			}*/
			AppendExplodeTiles(pBomb->ExplodeIdxs);
		}
		else if (pBomb->State == BombState::Over)
		{
			pBomb->Destroy();
			pBomb = nullptr;
		}
	}

	BombList.remove(nullptr);
}

void APlayMap::CheckExplodedBox()
{
	bool hasExploded = ExplodeTileIdxs.size() > 0;
	if (hasExploded)
	{
		for (size_t i = 0; i < ExplodeTileIdxs.size(); ++i)
		{
			FIntPoint boxIdx = ExplodeTileIdxs[i];
			MapBox->LaunchTileAnimAfterLoad(boxIdx, GlobalPath::ANIM_CRUMBLING_BOX);
		}
		ExplodeTileIdxs.clear();
	}
}

std::vector<FIntPoint> APlayMap::GetBombRange(const FIntPoint& _matIdx, const SBombTailTypes& _tailInfo)
{
	std::vector<FIntPoint> explodeIdxs;
	explodeIdxs.reserve(GlobalVar::MAX_BOMB_POWER * 4);
	explodeIdxs.push_back(_matIdx);

	for (size_t i = 0, size = _tailInfo.Up.size(); i < size; ++i)
	{
		explodeIdxs.push_back(_matIdx + FIntPoint::UP * (static_cast<int>(i) + 1));
	}
	for (size_t i = 0, size = _tailInfo.Down.size(); i < size; ++i)
	{
		explodeIdxs.push_back(_matIdx + FIntPoint::DOWN * (static_cast<int>(i) + 1));
	}
	for (size_t i = 0, size = _tailInfo.Left.size(); i < size; ++i)
	{
		explodeIdxs.push_back(_matIdx + FIntPoint::LEFT * (static_cast<int>(i) + 1));
	}
	for (size_t i = 0, size = _tailInfo.Right.size(); i < size; ++i)
	{
		explodeIdxs.push_back(_matIdx + FIntPoint::RIGHT * (static_cast<int>(i) + 1));
	}

	return explodeIdxs;
}

void APlayMap::AppendExplodeTiles(const std::vector<FIntPoint>& _appendIdxs)
{
	for (size_t i = 0; i < _appendIdxs.size(); ++i)
	{
		if (_appendIdxs[i].X < 0 || _appendIdxs[i].Y < 0)
		{
			continue;
		}

		ExplodeTileIdxs.push_back(_appendIdxs[i]);
	}
}