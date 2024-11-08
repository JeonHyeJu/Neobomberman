#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineBase/EngineSerializer.h>

/*
***Important!!
* It should match the order of the folder image.
*/
enum class TileType
{
	Ground = 0,
	Wall,
	Box,
};

class Tile : public ISerializObject
{
public:
	USpriteRenderer* SpriteRenderer = nullptr;

	bool IsMove = true;
	int SpriteIndex = -1;
	std::string SpriteName = "";
	FVector2D Scale;
	FVector2D Pivot;

	void Serialize(UEngineSerializer& _Ser)
	{
		if (SpriteRenderer != nullptr)
		{
			SpriteName = SpriteRenderer->GetCurSpriteName();
		}

		_Ser << IsMove;
		_Ser << SpriteIndex;
		_Ser << SpriteName;
		_Ser << Scale;
		_Ser << Pivot;
	}

	void DeSerialize(UEngineSerializer& _Ser)
	{
		int type = -1;
		_Ser >> IsMove;
		_Ser >> SpriteIndex;
		_Ser >> SpriteName;
		_Ser >> Scale;
		_Ser >> Pivot;
		
		//SpriteRenderer->SetSprite(SpriteName);	// This done in ATileMap
	}
};

class ATileMap : public AActor, public ISerializObject
{
public:
	ATileMap();
	~ATileMap();

	ATileMap(const ATileMap& _Other) = delete;
	ATileMap(ATileMap&& _Other) noexcept = delete;
	ATileMap& operator=(const ATileMap& _Other) = delete;
	ATileMap& operator=(ATileMap&& _Other) noexcept = delete;

	void Init(std::string_view _Sprite, const FIntPoint& _Count, const FVector2D& _TileSize, const TileType& _type);
	void SetTileWithLoc(FVector2D _Location, int _SpriteIndex, bool _isMove);
	void SetTile(const FIntPoint& _Index, int _SpriteIndex, bool _isMove);
	void SetTile(const FIntPoint& _Index, const FVector2D& _Pivot, const FVector2D& _SpriteScale, int _SpriteIndex, bool _isMove);

	FIntPoint LocationToMatrixIdx(const FVector2D& _loc);
	FVector2D MatrixIdxToLocation(const FIntPoint& _idx);
	FIntPoint LocationToIndex(const FVector2D& _loc);
	FVector2D IndexToLocation(const FIntPoint& _idx);

	inline FIntPoint GetTileIndex() const
	{
		return TileCount;
	}
	inline FVector2D GetTileSize() const
	{
		return TileSize;
	}
	inline FVector2D GetMapSize() const
	{
		return FVector2D({ TileCount.X * TileSize.X, TileCount.Y * TileSize.Y });
	}
	inline TileType GetType() const
	{
		return WholeTileType;
	}
	bool GetIsMovable(const FVector2D& _loc);

	Tile* GetTileRef(FIntPoint _idx);
	Tile* GetTileRef(FVector2D _loc);
	bool IsBlocked(const FVector2D& _loc);
	bool IsBlocked(const FIntPoint& _idx);

	bool IsIndexOver(FIntPoint _Index);

	void SetTilesAnimAfterLoad(std::string_view _animName, std::string_view _spriteName);
	void LaunchTileAnimAfterLoad(const FIntPoint& _pt, std::string_view _animName);

	void Serialize(UEngineSerializer& _Ser);
	void DeSerialize(UEngineSerializer& _Ser);

protected:

private:
	void DestroySpriteAfterLoad(const FIntPoint& _idx);

	std::string SpriteName = "";
	FIntPoint TileCount;
	FVector2D TileSize;
	TileType WholeTileType = TileType::Ground;
	std::vector<std::vector<Tile>> AllTiles;
};