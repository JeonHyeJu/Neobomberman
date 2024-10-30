#include "PreCompile.h"
#include "ImageManager.h"

#include <EngineBase/EnginePath.h>
#include <EngineBase/EngineDebug.h>
#include <EngineBase/EngineString.h>
#include <EngineCore/EngineAPICore.h>

UImageManager::UImageManager()
{
}

UImageManager::~UImageManager()
{
	{
		std::map<std::string, UEngineWinImage*>::iterator StartIter = Images.begin();
		std::map<std::string, UEngineWinImage*>::iterator EndIter = Images.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			UEngineWinImage* CurRes = StartIter->second;
			if (nullptr != CurRes)
			{
				delete CurRes;
				CurRes = nullptr;
			}
		}
	}

	{
		std::map<std::string, UEngineSprite*>::iterator StartIter = Sprites.begin();
		std::map<std::string, UEngineSprite*>::iterator EndIter = Sprites.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			UEngineSprite* CurRes = StartIter->second;
			if (nullptr != CurRes)
			{
				delete CurRes;
				CurRes = nullptr;
			}
		}
	}
}


void UImageManager::Load(std::string_view Path)
{
	UEnginePath EnginePath = UEnginePath(Path);

	std::string FileName = EnginePath.GetFileName();
	
	Load(FileName, Path);
}


void UImageManager::Load(std::string_view _KeyName, std::string_view Path)
{
	UEnginePath EnginePath = UEnginePath(Path);

	if (true == EnginePath.IsDirectory())
	{
		MSGASSERT("디렉토리는 로드할수 없습니다." + std::string(Path));
		return;
	}

	if (false == EnginePath.IsExists())
	{
		MSGASSERT("유효하지 않은 파일 경로 입니다." + std::string(Path));
		return;
	}

	UEngineWinImage* WindowImage = UEngineAPICore::GetCore()->GetMainWindow().GetWindowImage();

	std::string UpperName = UEngineString::ToUpper(_KeyName);
	UEngineWinImage* NewImage = new UEngineWinImage();
	NewImage->Load(WindowImage, Path);

	Images.insert({ UpperName , NewImage});

	UEngineSprite* NewSprite = new UEngineSprite();

	FTransform Trans;
	Trans.Location = { 0,0 };
	Trans.Scale = NewImage->GetImageScale();

	NewSprite->PushData(NewImage, Trans);

	Sprites.insert({ UpperName , NewSprite});
}

void UImageManager::CuttingSprite(std::string_view _KeyName, FVector2D _CuttingSize)
{
	std::string UpperName = UEngineString::ToUpper(_KeyName);

	if (false == Sprites.contains(UpperName))
	{
		MSGASSERT("존재하지 않은 스프라이트를 자르려고 했습니다" + std::string(_KeyName));
		return;
	}

	if (false == Images.contains(UpperName))
	{
		MSGASSERT("존재하지 않은 이미지를 기반으로 스프라이트를 자르려고 했습니다" + std::string(_KeyName));
		return;
	}

	UEngineSprite* Sprite = Sprites[UpperName];
	UEngineWinImage* Image = Images[UpperName];

	Sprite->ClearSpriteData();

	if (0 != (Image->GetImageScale().iX() % _CuttingSize.iX()))
	{
		MSGASSERT("스프라이트 컷팅에 x가 딱 떨어지지 않습니다." + std::string(_KeyName));
		return;
	}

	if (0 != (Image->GetImageScale().iY() % _CuttingSize.iY()))
	{
		MSGASSERT("스프라이트 컷팅에 y가 딱 떨어지지 않습니다." + std::string(_KeyName));
		return;
	}

	int SpriteX = Image->GetImageScale().iX() / _CuttingSize.iX();
	int SpriteY = Image->GetImageScale().iY() / _CuttingSize.iY();

	FTransform CuttingTrans;

	CuttingTrans.Location = FVector2D::ZERO;
	CuttingTrans.Scale = _CuttingSize;

	for (size_t y = 0; y < SpriteY; ++y)
	{
		for (size_t x = 0; x < SpriteX; ++x)
		{
			Sprite->PushData(Image, CuttingTrans);
			CuttingTrans.Location.X += _CuttingSize.X;
		}

		CuttingTrans.Location.X = 0.0f;
		CuttingTrans.Location.Y += _CuttingSize.Y;
	}
}

bool UImageManager::IsLoadSprite(std::string_view _KeyName)
{
	std::string UpperName = UEngineString::ToUpper(_KeyName);

	return Sprites.contains(UpperName);
}

UEngineSprite* UImageManager::FindSprite(std::string_view _KeyName)
{
	std::string UpperName = UEngineString::ToUpper(_KeyName);
	
	if (false == Sprites.contains(UpperName))
	{
		MSGASSERT("로드하지 않은 스프라이트를 사용하려고 했습니다" + std::string(_KeyName));
		return nullptr;
	}

	//std::map<std::string, UEngineSprite*>::iterator FindIter = Sprites.find(UpperName);
	return Sprites[UpperName];
}