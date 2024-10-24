#include "PreCompile.h"
#include "ContentsCore.h"
#include <EngineCore/EngineAPICore.h>

#include "PlayGameMode.h"
#include "Player.h"

ContentsCore::ContentsCore()
{

}

ContentsCore::~ContentsCore()
{

}

void ContentsCore::BeginPlay()
{
	//UEngineAPICore::GetCore()->CreateLevel("Title");
	UEngineAPICore::GetCore()->CreateLevel<APlayGameMode, APlayer>("Play");
	//UEngineAPICore::GetCore()->CreateLevel("End");

	UEngineAPICore::GetCore()->OpenLevel("Play");
}

void ContentsCore::Tick()
{
}