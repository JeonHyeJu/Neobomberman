#pragma once
#include <EnginePlatform/EngineWinImage.h>
#include <vector>
#include <EngineBase/EngineDebug.h>

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
		if (_Index >= Data.size())
		{
			MSGASSERT("스프라이트의 인덱스를 오버하여 사용하려고 했습니다." + GetName());
		}

		return Data[_Index];
	}

	void ClearSpriteData()
	{
		Data.clear();
	}

protected:

	std::vector<USpriteData> Data;
};


