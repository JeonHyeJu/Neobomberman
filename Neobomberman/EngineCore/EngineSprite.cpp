#include "PreCompile.h"
#include "EngineSprite.h"
#include <EngineBase/EngineDebug.h>

UEngineSprite::UEngineSprite()
{
}

UEngineSprite::~UEngineSprite()
{
}


void UEngineSprite::PushData(UEngineWinImage* _Image, const FTransform& _Trans)
{
	if (true == _Trans.Scale.IsZeroed())
	{
		MSGASSERT("크기가 0인 이미지를 만들수 없습니다");
	}
	
	Data.push_back({ _Image, _Trans });
}


