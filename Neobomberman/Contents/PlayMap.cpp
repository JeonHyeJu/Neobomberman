#include "PreCompile.h"
#include "PlayMap.h"
#include <EngineCore/EngineAPICore.h>

APlayMap::APlayMap()
{
	FVector2D WindowSize =  UEngineAPICore::GetCore()->GetMainWindow().GetWindowSize();

	SetActorScale(WindowSize.Half());
	SetActorLocation(WindowSize.Half());
}

APlayMap::~APlayMap()
{
}

