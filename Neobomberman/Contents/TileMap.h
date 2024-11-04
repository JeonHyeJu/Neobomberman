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
	int TileType = -1;
	FVector2D Scale;
	FVector2D Pivot;
	int SpriteIndex = -1;

	void Serialize(UEngineSerializer& _Ser)
	{
		std::string SpriteName;
		if (nullptr != SpriteRenderer)
		{
			SpriteName = SpriteRenderer->GetCurSpriteName();
		}
		_Ser << SpriteName;
		_Ser << IsMove;
		_Ser << TileType;
		_Ser << Scale;
		_Ser << Pivot;
		_Ser << SpriteIndex;
	}

	void DeSerialize(UEngineSerializer& _Ser)
	{
		//std::string SpriteName;
		// _Ser >> SpriteName;

		// SpriteRenderer->SetSprite(SpriteName);

		std::string SpriteName;
		_Ser >> SpriteName;
		_Ser >> IsMove;
		_Ser >> TileType;
		_Ser >> Scale;
		_Ser >> Pivot;
		_Ser >> SpriteIndex;
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
	void SetSpriteAndIndex(const FIntPoint& _Index, int _SpriteIndex);
	void SetSpriteAndIndex(const FIntPoint& _Index, const FVector2D& _Pivot, const FVector2D& _SpriteScale, int _SpriteIndex);
	void SetSpriteAndIndexWithLocation(FVector2D _Location, int _SpriteIndex);
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

	Tile* GetTileRef(FIntPoint _Index);
	Tile* GetTileRef(FVector2D _Location);

	bool IsIndexOver(FIntPoint _Index);

	void Serialize(UEngineSerializer& _Ser);
	void DeSerialize(UEngineSerializer& _Ser);

	inline TileType GetType() const
	{
		return Type;
	}

protected:

private:
	std::string SpriteName;
	FIntPoint TileCount;
	FVector2D TileSize;
	std::vector<std::vector<Tile>> AllTiles;
	TileType Type;
};


