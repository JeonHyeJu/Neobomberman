#include "PreCompile.h"
#include "ContentsEnum.h"
#include "TileMap.h"

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
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetOrder(_Index.Y);

	FVector2D loc = IndexToLocation(_Index);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetComponentLocation(loc + TileSize.Half() + _Pivot);
	AllTiles[_Index.Y][_Index.X].Pivot = _Pivot;
	AllTiles[_Index.Y][_Index.X].Scale = _SpriteScale;
	AllTiles[_Index.Y][_Index.X].SpriteIndex = _SpriteIndex;
	AllTiles[_Index.Y][_Index.X].IsMove = _isMove;
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
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->CreateAnimation("PortalOpened", "OpenedPortal.png", 0, 4, .5f, false);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->CreateAnimation("PortalMove", "OpenedPortalMove.png", 0, 2, .5f);
	AllTiles[_Index.Y][_Index.X].SpriteRenderer->SetAnimationEvent("PortalOpened", 4, std::bind(&ATileMap::OnRunPortalMove, this));

	AllTiles[_Index.Y][_Index.X].SpriteRenderer->ChangeAnimation("PortalNormal");

	PortalIdx = _Index;
}

void ATileMap::OpenPortal()
{
	AllTiles[PortalIdx.Y][PortalIdx.X].SpriteRenderer->ChangeAnimation("PortalOpened");
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

bool ATileMap::IsBlocked(const FVector2D& _loc)
{
	FIntPoint idx = LocationToIndex(_loc - GetActorLocation());
	return IsBlocked(idx);
}

bool ATileMap::IsBlocked(const FIntPoint& _idx)
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
			AllTiles[y][x].SpriteRenderer->SetAnimationEvent(_animName, 10, [=]() { DestroySpriteAfterLoad(FIntPoint({ static_cast<int>(x), static_cast<int>(y) })); });
			AllTiles[y][x].SpriteRenderer->SetOrder(ERenderOrder::CRUMBLING_BOX);
		}
	}
}

void ATileMap::LaunchTileAnimAfterLoad(const FIntPoint& _idx, std::string_view _animName)
{
	if (IsIndexOver(_idx) == true)
	{
		return;
	}

	if (AllTiles[_idx.Y][_idx.X].SpriteRenderer != nullptr)
	{
		AllTiles[_idx.Y][_idx.X].SpriteRenderer->ChangeAnimation(_animName);
	}
}

void ATileMap::DestroySpriteAfterLoad(const FIntPoint& _idx)
{
	if (IsIndexOver(_idx) == true)
	{
		return;
	}

	// AllTiles[_idx.Y][_idx.X].SpriteRenderer will not be nullptr.
	// SetTilesAnim() set animation to exist renderer.
	AllTiles[_idx.Y][_idx.X].SpriteRenderer->Destroy();
	AllTiles[_idx.Y][_idx.X].SpriteRenderer = nullptr;
	AllTiles[_idx.Y][_idx.X].IsMove = true;
}

void ATileMap::OnRunPortalMove()
{
	IsPortalOpened = true;
	AllTiles[PortalIdx.Y][PortalIdx.X].SpriteRenderer->ChangeAnimation("PortalMove");
}