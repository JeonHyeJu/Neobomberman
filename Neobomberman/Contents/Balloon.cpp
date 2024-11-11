#include "PreCompile.h"
#include "Balloon.h"
#include <EngineCore/SpriteRenderer.h>

ABalloon::ABalloon()
{

}

ABalloon::~ABalloon()
{

}

void ABalloon::BeginPlay()
{
	Super::BeginPlay();
}

void ABalloon::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}

void ABalloon::Init(std::string_view _spritePath)
{
	AMonster::Init(_spritePath);

	SpriteRenderer->CreateAnimation("Run", _spritePath, 0, 8, 0.5f);
	SpriteRenderer->ChangeAnimation("Run");
}