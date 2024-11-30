#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineBase/EngineSerializer.h>
#include "ContentsEnum.h"

/*
***Important!!
* It should match the order of the folder image.
*/
enum class TileType
{
	Ground = 0,
	Wall,
	Box,
	Cover,
};

class Tile : public ISerializObject
{
public:
	USpriteRenderer* SpriteRenderer = nullptr;
	USpriteRenderer* SpriteRendererItem = nullptr;

	bool IsMove = true;
	int SpriteIndex = -1;
	std::string SpriteName = "";
	FVector2D Scale;
	FVector2D Pivot;
	EItem Item = EItem::NONE;

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
		
		//SpriteRenderer->SetSprite(SpriteName);	// This is done in ATileMap
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

	// must be called before Deserialize.
	void SetCustomOrder(ERenderOrder _order);
	void Init(std::string_view _Sprite, const FIntPoint& _Count, const FVector2D& _TileSize, const TileType& _type);
	void SetTileWithLoc(FVector2D _Location, int _SpriteIndex, bool _isMove);
	void SetTile(const FIntPoint& _Index, int _SpriteIndex, bool _isMove);
	void SetTile(const FIntPoint& _Index, const FVector2D& _Pivot, const FVector2D& _SpriteScale, int _SpriteIndex, bool _isMove);
	void SetPortal(const FIntPoint& _Index, const FVector2D& _Pivot, const FVector2D& _SpriteScale, std::string_view _spriteName);
	void OpenPortal();

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
	inline bool GetIsPortalOpened() const
	{
		return IsPortalOpened;
	}
	bool HasItem(const FIntPoint& _idx);
	bool IsShowingItem(const FIntPoint& _idx);
	EItem GetItem(const FIntPoint& _idx);
	EItem PopItem(const FIntPoint& _idx);
	bool GetIsMovable(const FVector2D& _loc);
	bool GetIsMovable(const FIntPoint& _idx);

	Tile* GetTileRef(FIntPoint _idx);
	Tile* GetTileRef(FVector2D _loc);
	bool HasTileSprite(const FVector2D& _loc);
	bool HasTileSprite(const FIntPoint& _idx);

	bool IsIndexOver(FIntPoint _Index);
	bool IsEdge(FIntPoint _Index);

	void SetItemsAfterLoad(const std::vector<EItem>& _items);
	void SetTilesAnimAfterLoad(std::string_view _animName, std::string_view _spriteName, int _firstIdx, int _lastIdx);
	void LaunchTileAnimAfterLoad(const FIntPoint& _pt, std::string_view _animName);

	void Serialize(UEngineSerializer& _Ser);
	void DeSerialize(UEngineSerializer& _Ser);
	void DestroySpriteItem(const FIntPoint& _idx);

	/* Cheats */
	void CheatDestoyAllBoxes();

protected:

private:
	void DestroySprite(const FIntPoint& _idx);
	void OnEndCrumbling(const FIntPoint& _idx);
	void OnRunPortalMove();

	const char* ITEM_SPRITE_NAME = "items.png";

	std::string SpriteName = "";
	FIntPoint TileCount;
	FVector2D TileSize;
	TileType WholeTileType = TileType::Ground;
	std::vector<std::vector<Tile>> AllTiles;
	std::vector<FIntPoint> TileIdxs;
	std::list<FIntPoint> HavingItemTileIdxs;

	bool IsPortalOpened = false;
	FIntPoint PortalIdx;

	ERenderOrder CustomOrder = ERenderOrder::NONE;
};