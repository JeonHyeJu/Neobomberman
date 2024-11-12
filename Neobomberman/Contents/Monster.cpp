#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "Monster.h"
#include "PlayMap.h"
#include <EngineCore/SpriteRenderer.h>

AMonster::AMonster()
{

}

AMonster::~AMonster()
{

}

void AMonster::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void AMonster::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}

void AMonster::SetCurMap(APlayMap* _map)
{
	CurMap = _map;
	PathFinder.SetData(CurMap);
}