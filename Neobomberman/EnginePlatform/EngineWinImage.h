#pragma once
#include <Windows.h>
#include <EngineBase/EngineMath.h>
#include <EngineBase/Object.h>


class UEngineWinImage : public UObject
{
public:
	UEngineWinImage();
	~UEngineWinImage();

	UEngineWinImage(const UEngineWinImage& _Other) = delete;
	UEngineWinImage(UEngineWinImage&& _Other) noexcept = delete;
	UEngineWinImage& operator=(const UEngineWinImage& _Other) = delete;
	UEngineWinImage& operator=(UEngineWinImage&& _Other) noexcept = delete;

	HDC GetDC()
	{
		return ImageDC;
	}
	
	void Create(HDC _DC)
	{
		ImageDC = _DC;
	}

	void Create(UEngineWinImage* _TargetImage, FVector2D _Scale);

	void CopyToBit(UEngineWinImage* _TargetImage, 
		const FTransform& _Trans);
	void CopyToTrans(UEngineWinImage* _TargetImage, 
		const FTransform& _RenderTrans, 
		const FTransform& _LTImageTrans,
		UColor _Color = UColor(255, 0, 255, 0));

	void Load(UEngineWinImage* _TargetImage, std::string_view _Path);

	FVector2D GetImageScale() const
	{
		return { Info.bmWidth, Info.bmHeight };
	}

protected:

private:
	HDC ImageDC = nullptr;
	HBITMAP hBitMap = nullptr;

	BITMAP Info;
};

