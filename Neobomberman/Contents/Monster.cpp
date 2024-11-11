#include "PreCompile.h"
#include "Monster.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include <EngineCore/SpriteRenderer.h>

AMonster::AMonster()
{

}

AMonster::~AMonster()
{

}

void AMonster::Init(std::string_view _spritePath)
{
	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(_spritePath);
	SpriteRenderer->SetComponentLocation({ 0, 0 });
	SpriteRenderer->SetComponentScale(GlobalVar::BOMBERMAN_SIZE);
	SpriteRenderer->SetPivotType(PivotType::Bot);
	SpriteRenderer->SetOrder(ERenderOrder::PLAYER);
}