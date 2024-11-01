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

#include "ActorComponent.h"

std::list<UActorComponent*> AActor::ComponentList;

void AActor::ComponentBeginPlay()
{
	{
		std::list<UActorComponent*>::iterator StartIter = ComponentList.begin();
		std::list<UActorComponent*>::iterator EndIter = ComponentList.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			UActorComponent* CurActor = *StartIter;
			CurActor->BeginPlay();
		}

		ComponentList.clear();
	}
}

AActor::AActor()
{
}

AActor::~AActor()
{
	std::list<UActorComponent*>::iterator StartIter = Components.begin();
	std::list<UActorComponent*>::iterator EndIter = Components.end();
	for (; StartIter != EndIter; ++StartIter)
	{
		UActorComponent* Component = *StartIter;

		if (nullptr != Component)
		{
			delete Component;
		}
	}

	Components.clear();
}