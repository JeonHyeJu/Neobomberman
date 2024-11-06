#include "PreCompile.h"
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
		AllTiles[y].resize(_Count.X);;
	}
}

void ATileMap::SetTile(const FIntPoint& _Index, int _SpriteIndex, bool _isMove)
{
	SetTile(_Index, { 0, 0 }, TileSize, _SpriteIndex, _isMove);
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

FVector2D ATileMap::IndexToLocation(const FIntPoint& _Index)
{
	return FVector2D(_Index.X * TileSize.X, _Index.Y * TileSize.Y);
}

FIntPoint ATileMap::LocationToIndex(const FVector2D& _Location)
{
	FVector2D Location = _Location / TileSize;

	return FIntPoint(Location.iX(), Location.iY());
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

Tile* ATileMap::GetTileRef(FVector2D _Location)
{
	FVector2D loc = GetActorLocation();
	FIntPoint Point = LocationToIndex(_Location - loc);

	return GetTileRef(Point);
}

Tile* ATileMap::GetTileRef(FIntPoint _Index)
{
	if (true == IsIndexOver(_Index))
	{
		return nullptr;
	}

	return &AllTiles[_Index.Y][_Index.X];
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