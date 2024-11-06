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
	USpriteRenderer* SpriteRenderer;

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
	void SetTile(const FIntPoint& _Index, int _SpriteIndex, bool _isMove);
	void SetTile(const FIntPoint& _Index, const FVector2D& _Pivot, const FVector2D& _SpriteScale, int _SpriteIndex, bool _isMove);
	void SetTileWithLoc(FVector2D _Location, int _SpriteIndex, bool _isMove);
	FVector2D IndexToLocation(const FIntPoint& _Index);
	FIntPoint LocationToIndex(const FVector2D& _Location);

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

	Tile* GetTileRef(FIntPoint _Index);
	Tile* GetTileRef(FVector2D _Location);

	bool IsIndexOver(FIntPoint _Index);

	void Serialize(UEngineSerializer& _Ser);
	void DeSerialize(UEngineSerializer& _Ser);

protected:

private:
	std::string SpriteName = "";
	FIntPoint TileCount;
	FVector2D TileSize;
	TileType WholeTileType = TileType::Ground;
	std::vector<std::vector<Tile>> AllTiles;
};