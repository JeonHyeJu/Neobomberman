#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "TileMap.h"
#include <EngineBase/EngineRandom.h>

ATileMap::ATileMap()
{
}

ATileMap::~ATileMap()
{
}

void ATileMap::Init(std::string_view _Sprite, const FIntPoint& _Count, const FVector2D& _TileSize, const TileType& _type)
{
	SpriteName = _Sprite;
	TileCount = _Count;
	TileSize = _TileSize;
	WholeTileType = _type;

	AllTiles.resize(_Count.Y);

	for (size_t y = 0; y < AllTiles.size(); y++)
	{
		AllTiles[y].resize(_Count.X);
	}

	TileIdxs.reserve(_Count.X * _Count.Y);
}

void ATileMap::SetCustomOrder(ERenderOrder _order)
{
	CustomOrder = _order;
}

// from mouse or player location
void ATileMap::SetTileWithLoc(FVector2D _Location, int _SpriteIndex, bool _isMove)
{
	FVector2D TilePos = _Location - GetActorLocation();
	FIntPoint Point = LocationToIndex(TilePos);

	if (true == IsIndexOver(Point))
	{
		return;
	}

	SetTile(Point, _SpriteIndex, _isMove);
}

void ATileMap::SetTile(const FIntPoint& _Index, int _SpriteIndex, bool _isMove)
{
	SetTile(_Index, { 0, 0 }, TileSize, _SpriteIndex, _isMove);
}

void ATileMap::SetTile(const FIntPoint& _Index, const FVector2D& _Pivot, const FVector2D& _SpriteScale, int _SpriteIndex, bool _isMove)
{
	if (IsIndexOver(_Index) == true)
	{
		return;
	}

	if (AllTiles[_Index.Y][_Index.X].SpriteRenderer == nullptr)
	{
		AllTiles[_Index.Y][_Index.X].SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	}

	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetSprite(SpriteName, _SpriteIndex);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetComponentScale(_SpriteScale);
	if (CustomOrder != ERenderOrder::NONE)
	{
		AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetOrder(CustomOrder);
	}
	else
	{
		AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetOrder(_Index.Y);
	}

	FVector2D loc = IndexToLocation(_Index);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetComponentLocation(loc + TileSize.Half() + _Pivot);
	AllTiles[_Index.Y][_Index.X].Pivot = _Pivot;
	AllTiles[_Index.Y][_Index.X].Scale = _SpriteScale;
	AllTiles[_Index.Y][_Index.X].SpriteIndex = _SpriteIndex;
	AllTiles[_Index.Y][_Index.X].IsMove = _isMove;

	TileIdxs.push_back(_Index);
}

void ATileMap::SetPortal(const FIntPoint& _Index, const FVector2D& _Pivot, const FVector2D& _SpriteScale, std::string_view _spriteName)
{
	if (IsIndexOver(_Index) == true)
	{
		return;
	}

	if (AllTiles[_Index.Y][_Index.X].SpriteRenderer == nullptr)
	{
		AllTiles[_Index.Y][_Index.X].SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	}

	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetSprite(_spriteName);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetComponentScale(_SpriteScale);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetOrder(_Index.Y);

	FVector2D loc = IndexToLocation(_Index);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetComponentLocation(loc + TileSize.Half() + _Pivot);
	AllTiles[_Index.Y][_Index.X].Pivot = _Pivot;
	AllTiles[_Index.Y][_Index.X].Scale = _SpriteScale;
	AllTiles[_Index.Y][_Index.X].SpriteIndex = 0;
	AllTiles[_Index.Y][_Index.X].IsMove = true;

	AllTiles[_Index.Y][_Index.X].SpriteRenderer->CreateAnimation("PortalNormal", "ClosedPortal.png", 0, 1, 1.f);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->CreateAnimation("PortalOpened", "OpenedPortal.png", 0, 4, .25f, false);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->CreateAnimation("PortalMove", "OpenedPortalMove.png", 0, 2, .25f);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetAnimationEvent("PortalOpened", 4, std::bind(&ATileMap::OnRunPortalMove, this));

	AllTiles[_Index.Y][_Index.X].SpriteRenderer->ChangeAnimation("PortalNormal");

	PortalIdx = _Index;
}

void ATileMap::SetItemsAfterLoad(const std::vector<EItem>& _items)
{
	UEngineRandom Random;

	size_t itemSize = _items.size();
	//std::vector<int> vals = Random.RandomIntMultiple(0, static_cast<int>(TileIdxs.size())-1, static_cast<int>(itemSize));
	std::vector<int> vals = { 0, 1, 2 };

	HavingItemTileIdxs.clear();
	const int ITEM_SIZE = 2;	// Temp

	for (int i = 0, size = static_cast<int>(vals.size()); i < size; ++i)
	{
		FIntPoint idx = TileIdxs[vals[i]];
		HavingItemTileIdxs.push_back(idx);

		int itemSpriteIdx = static_cast<int>(_items[i]);
		if (AllTiles[idx.Y][idx.X].SpriteRendererItem == nullptr)
		{
			AllTiles[idx.Y][idx.X].SpriteRendererItem = CreateDefaultSubObject<USpriteRenderer>();
		}
		AllTiles[idx.Y][idx.X].SpriteRendererItem->SetSprite(ITEM_SPRITE_NAME);
		AllTiles[idx.Y][idx.X].SpriteRendererItem->SetComponentScale(GlobalVar::BOMBERMAN_SIZE);
		AllTiles[idx.Y][idx.X].SpriteRendererItem->SetComponentLocation(AllTiles[idx.Y][idx.X].SpriteRenderer->GetComponentLocation());
		AllTiles[idx.Y][idx.X].SpriteRendererItem->CreateAnimation(GlobalPath::ANIM_ITEM, ITEM_SPRITE_NAME, ITEM_SIZE*itemSpriteIdx, ITEM_SIZE*itemSpriteIdx + 1, 0.5f);
		AllTiles[idx.Y][idx.X].SpriteRendererItem->SetOrder(ERenderOrder::CRUMBLING_BOX);
		AllTiles[idx.Y][idx.X].SpriteRendererItem->SetActive(false);
		AllTiles[idx.Y][idx.X].Item = _items[i];
	}
}

void ATileMap::OpenPortal()
{
	if (AllTiles[PortalIdx.Y][PortalIdx.X].SpriteRenderer->GetCurAnimName() != "PortalOpened")
	{
		AllTiles[PortalIdx.Y][PortalIdx.X].SpriteRenderer->ChangeAnimation("PortalOpened");
	}
}

bool ATileMap::IsIndexOver(FIntPoint _Index)
{
	if (0 > _Index.X)
	{
		return true;
	}

	if (0 > _Index.Y)
	{
		return true;
	}

	if (TileCount.X - 1 < _Index.X)
	{
		return true;
	}

	if (TileCount.Y - 1 < _Index.Y)
	{
		return true;
	}

	return false;
}

bool ATileMap::IsEdge(FIntPoint _Index)
{
	if (_Index.X == 0 || _Index.Y == 0)
	{
		return true;
	}
	if (_Index.X == (TileCount.X - 1) || _Index.Y == (TileCount.Y - 1))
	{
		return true;
	}

	return false;
}

Tile* ATileMap::GetTileRef(FVector2D _loc)
{
	FIntPoint idx = LocationToMatrixIdx(_loc);
	return GetTileRef(idx);
}

Tile* ATileMap::GetTileRef(FIntPoint _idx)
{
	if (true == IsIndexOver(_idx))
	{
		return nullptr;
	}

	return &AllTiles[_idx.Y][_idx.X];
}

bool ATileMap::HasTileSprite(const FVector2D& _loc)
{
	FIntPoint idx = LocationToIndex(_loc - GetActorLocation());
	return HasTileSprite(idx);
}

bool ATileMap::HasTileSprite(const FIntPoint& _idx)
{
	if (IsIndexOver(_idx) == true)
	{
		return true;
	}

	bool hasTile = AllTiles[_idx.Y][_idx.X].SpriteRenderer != nullptr;	// Temp
	return hasTile;
}

void ATileMap::Serialize(UEngineSerializer& _Ser)
{
	int type = static_cast<int>(WholeTileType);

	_Ser << SpriteName;
	_Ser << TileCount;
	_Ser << TileSize;
	_Ser << type;
	_Ser << AllTiles;
}

void ATileMap::DeSerialize(UEngineSerializer& _Ser)
{
	std::string loadSpriteName = "";
	FIntPoint loadTileCount;
	FVector2D loadTileSize;
	int loadTileType;
	std::vector<std::vector<Tile>> loadTiles;

	_Ser >> loadSpriteName;
	_Ser >> loadTileCount;
	_Ser >> loadTileSize;
	_Ser >> loadTileType;
	_Ser >> loadTiles;

	Init(loadSpriteName, loadTileCount, loadTileSize, static_cast<TileType>(loadTileType));

	for (int y = 0; y < loadTiles.size(); y++)
	{
		for (int x = 0; x < loadTiles[y].size(); x++)
		{
			if (loadTiles[y][x].SpriteIndex != -1)
			{
				SetTile({ x, y }, loadTiles[y][x].Pivot, loadTiles[y][x].Scale, loadTiles[y][x].SpriteIndex, loadTiles[y][x].IsMove);
			}
		}
	}
}

bool ATileMap::GetIsMovable(const FVector2D& _loc)
{
	Tile* tile = GetTileRef(_loc);
	if (tile != nullptr)
	{
		return tile->IsMove;
	}

	return false;
}

FIntPoint ATileMap::LocationToMatrixIdx(const FVector2D& _loc)
{
	FIntPoint idx = LocationToIndex(_loc - GetActorLocation());
	return idx;
}

FVector2D ATileMap::MatrixIdxToLocation(const FIntPoint& _idx)
{
	FVector2D loc = IndexToLocation(_idx) + GetActorLocation();
	return loc;
}

FVector2D ATileMap::IndexToLocation(const FIntPoint& _idx)
{
	return FVector2D(_idx.X * TileSize.X, _idx.Y * TileSize.Y);
}

FIntPoint ATileMap::LocationToIndex(const FVector2D& _loc)
{
	FVector2D Location = _loc / TileSize;

	return FIntPoint(Location.iX(), Location.iY());
}

/* after loading */
void ATileMap::SetTilesAnimAfterLoad(std::string_view _animName, std::string_view _spriteName)
{
	size_t ySize = AllTiles.size();
	for (size_t y = 0; y < ySize; y++)
	{
		size_t xSize = AllTiles[y].size();
		for (size_t x = 0; x < xSize; x++)
		{
			if (AllTiles[y][x].SpriteRenderer == nullptr)
			{
				continue;
			}

			// Temp
			AllTiles[y][x].SpriteRenderer->CreateAnimation(_animName, _spriteName, 0, 10, 0.15f, false);
			AllTiles[y][x].SpriteRenderer->SetAnimationEvent(_animName, 10, 
				[=]()
				{
					OnEndCrumbling(FIntPoint({ static_cast<int>(x), static_cast<int>(y) }));
				});
			AllTiles[y][x].SpriteRenderer->SetOrder(ERenderOrder::CRUMBLING_BOX);
		}
	}
}

bool ATileMap::HasItem(const FIntPoint& _idx)
{
	if (IsIndexOver(_idx))
	{
		return false;
	}

	return (AllTiles[_idx.Y][_idx.X].Item != EItem::NONE);
}

EItem ATileMap::GetItem(const FIntPoint& _idx)
{
	if (IsIndexOver(_idx))
	{
		return EItem::NONE;
	}

	return AllTiles[_idx.Y][_idx.X].Item;
}

EItem ATileMap::PopItem(const FIntPoint& _idx)
{
	EItem ret = GetItem(_idx);
	AllTiles[_idx.Y][_idx.X].Item = EItem::NONE;
	DestroySpriteItem(_idx);
	return ret;
}

void ATileMap::LaunchTileAnimAfterLoad(const FIntPoint& _idx, std::string_view _animName)
{
	if (IsIndexOver(_idx))
	{
		return;
	}

	if (AllTiles[_idx.Y][_idx.X].SpriteRenderer != nullptr)
	{
		AllTiles[_idx.Y][_idx.X].SpriteRenderer->ChangeAnimation(_animName);
	}
}

void ATileMap::DestroySprite(const FIntPoint& _idx)
{
	if (IsIndexOver(_idx)) return;

	AllTiles[_idx.Y][_idx.X].IsMove = true;

	if (AllTiles[_idx.Y][_idx.X].SpriteRenderer == nullptr) return;

	// AllTiles[_idx.Y][_idx.X].SpriteRenderer will not be nullptr.
	// SetTilesAnim() set animation to exist renderer.
	AllTiles[_idx.Y][_idx.X].SpriteRenderer->Destroy();
	AllTiles[_idx.Y][_idx.X].SpriteRenderer = nullptr;
}

void ATileMap::DestroySpriteItem(const FIntPoint& _idx)
{
	if (IsIndexOver(_idx)) return;
	if (AllTiles[_idx.Y][_idx.X].SpriteRendererItem == nullptr) return;

	std::list<FIntPoint>::iterator it = HavingItemTileIdxs.begin();
	std::list<FIntPoint>::iterator itEnd = HavingItemTileIdxs.end();

	for (; it != itEnd; ++it)
	{
		if (*it == _idx)
		{
			AllTiles[_idx.Y][_idx.X].SpriteRendererItem->Destroy();
			AllTiles[_idx.Y][_idx.X].SpriteRendererItem = nullptr;
			it = HavingItemTileIdxs.erase(it);

			if (it == itEnd) break;
		}
	}
}

void ATileMap::OnEndCrumbling(const FIntPoint& _idx)
{
	DestroySprite(_idx);

	if (HasItem(_idx))
	{
		if (AllTiles[_idx.Y][_idx.X].SpriteRendererItem != nullptr)
		{
			AllTiles[_idx.Y][_idx.X].SpriteRendererItem->SetActive(true);
			AllTiles[_idx.Y][_idx.X].SpriteRendererItem->ChangeAnimation(GlobalPath::ANIM_ITEM);
		}
	}
}

void ATileMap::OnRunPortalMove()
{
	IsPortalOpened = true;
	AllTiles[PortalIdx.Y][PortalIdx.X].SpriteRenderer->ChangeAnimation("PortalMove");
}

/* Cheats */
void ATileMap::CheatDestoyAllBoxes()
{
	size_t ySize = AllTiles.size();
	for (size_t y = 0; y < ySize; y++)
	{
		size_t xSize = AllTiles[y].size();
		for (size_t x = 0; x < xSize; x++)
		{
			if (AllTiles[y][x].SpriteRenderer == nullptr)
			{
				continue;
			}

			AllTiles[y][x].SpriteRenderer->ChangeAnimation(GlobalPath::ANIM_CRUMBLING_BOX);
		}
	}
}