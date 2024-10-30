#pragma once
#include <EnginePlatform/EngineWinImage.h>
#include <vector>

class UEngineSprite : public UObject
{
public:
	class USpriteData
	{
	public:
		UEngineWinImage* Image;
		FTransform Transform;
	};

	UEngineSprite();
	~UEngineSprite();

	UEngineSprite(const UEngineSprite& _Other) = delete;
	UEngineSprite(UEngineSprite&& _Other) noexcept = delete;
	UEngineSprite& operator=(const UEngineSprite& _Other) = delete;
	UEngineSprite& operator=(UEngineSprite&& _Other) noexcept = delete;

	void PushData(UEngineWinImage* Image, const FTransform& _Trans);

	USpriteData GetSpriteData(int _Index = 0)
	{
		return Data[_Index];
	}

	void ClearSpriteData()
	{
		Data.clear();
	}

protected:

	std::vector<USpriteData> Data;
};


