#include "PreCompile.h"
#include "Mushroom.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include <EngineCore/SpriteRenderer.h>

AMushroom::AMushroom()
{
}

AMushroom::~AMushroom()
{
}

void AMushroom::BeginPlay()
{
	Super::BeginPlay();
}

void AMushroom::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}

void AMushroom::Init(std::string_view _spritePath)
{
	AMonster::Init(_spritePath);

	SpriteRenderer->CreateAnimation("Idle_Up", _spritePath, 0, 0, 0.1f);
	SpriteRenderer->CreateAnimation("Idle_Down", _spritePath, 22, 22, 0.1f);
	SpriteRenderer->CreateAnimation("Idle_Left", _spritePath, 33, 33, 0.1f);
	SpriteRenderer->CreateAnimation("Idle_Right", _spritePath, 11, 11, 0.1f);

	SpriteRenderer->CreateAnimation("Run_Up", _spritePath, 1, 6, 0.1f);
	SpriteRenderer->CreateAnimation("Run_Down", _spritePath, 23, 28, 0.1f);
	SpriteRenderer->CreateAnimation("Run_Left", _spritePath, 34, 39, 0.1f);
	SpriteRenderer->CreateAnimation("Run_Right", _spritePath, 12, 17, 0.1f);

	std::vector<int> indexes;
	std::vector<float> times;

	// temp. TODO
	int startIdx = 44;
	int endIdx = 54;
	int size = endIdx - startIdx;
	indexes.reserve(size);
	times.reserve(size);
	for (int i = startIdx; i < endIdx; i++)
	{
		indexes.push_back(i);
		if (i >= startIdx+3 && i < endIdx-3)
		{
			times.push_back(1.f);
		}
		else
		{
			times.push_back(0.3f);
		}
		
	}
	SpriteRenderer->CreateAnimation("Jump", _spritePath, indexes, times, 0.5f);

	//SpriteRenderer->ChangeAnimation("Idle_Down");
	SpriteRenderer->ChangeAnimation("Jump");
}