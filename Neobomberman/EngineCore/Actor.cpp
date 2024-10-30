#include "PreCompile.h"
#include "Actor.h"

#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineMath.h>
#include <EngineBase/EngineDebug.h>
#include <EnginePlatform/EngineWindow.h>
#include <EnginePlatform/EngineWinImage.h>

#include "EngineSprite.h"
#include <EngineBase/EngineDebug.h>

#include "ImageManager.h"


AActor::AActor()
{
}

AActor::~AActor()
{
}


void AActor::Render()
{
	if (nullptr == Sprite)
	{
		MSGASSERT("스프라이트가 세팅되지 않은 액터를 랜더링을 할수 없습니다.");
		return;
	}

	UEngineWindow& MainWindow = UEngineAPICore::GetCore()->GetMainWindow();
	UEngineWinImage* BackBufferImage = MainWindow.GetBackBuffer();

	UEngineSprite::USpriteData CurData = Sprite->GetSpriteData(CurIndex);
	CurData.Image;
	CurData.Transform;
	CurData.Image->CopyToTrans(BackBufferImage, Transform, CurData.Transform);

}
void AActor::SetSprite(std::string_view _Name, int _CurIndex /*= 0*/)
{
	Sprite = UImageManager::GetInst().FindSprite(_Name);

	if (nullptr == Sprite)
	{
		MSGASSERT("로드하지 않은 스프라이트를 사용하려고 했습니다" + std::string(_Name));
		return;
	}

	CurIndex = _CurIndex;
}