#include "PreCompile.h"
#include "Mushroom.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/PathFindAStar.h>
//#include "DebugLog.h"	// TODO: delete

AMushroom::AMushroom()
{
	Speed = 20.f;
	Random.SetSeed(MonsterIdx);

	Fsm.CreateState(EMonsterState::JUMPING, std::bind(&AMushroom::Jumping, this, std::placeholders::_1));
}

AMushroom::~AMushroom()
{
}

void AMushroom::BeginPlay()
{
	AMonster::BeginPlay();
}

void AMushroom::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);
}

void AMushroom::Init()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;
	const float animTime = 0.5f;

	SpriteRenderer = CreateDefaultSubObject<USpriteRenderer>();
	SpriteRenderer->SetSprite(SPRITE_NAME);
	SpriteRenderer->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SpriteRenderer->SetComponentScale(size);
	SpriteRenderer->SetPivotType(PivotType::Bot);
	SpriteRenderer->SetOrder(ERenderOrder::MONSTER);

	SpriteRenderer->CreateAnimation("Idle_Up", SPRITE_NAME, 0, 0, animTime);
	SpriteRenderer->CreateAnimation("Idle_Down", SPRITE_NAME, 22, 22, animTime);
	SpriteRenderer->CreateAnimation("Idle_Left", SPRITE_NAME, 33, 33, animTime);
	SpriteRenderer->CreateAnimation("Idle_Right", SPRITE_NAME, 11, 11, animTime);

	SpriteRenderer->CreateAnimation("Run_Up", SPRITE_NAME, 1, 6, animTime);
	SpriteRenderer->CreateAnimation("Run_Down", SPRITE_NAME, 23, 28, animTime);
	SpriteRenderer->CreateAnimation("Run_Left", SPRITE_NAME, 34, 39, animTime);
	SpriteRenderer->CreateAnimation("Run_Right", SPRITE_NAME, 12, 17, animTime);

	std::vector<int> indexes;
	std::vector<float> times;

	int jumpAnimSize = IDX_ANIM_END - IDX_ANIM_START;
	indexes.reserve(jumpAnimSize);
	times.reserve(jumpAnimSize);

	const int IDX_LONG_START = IDX_ANIM_START + 3;
	const int IDX_LONG_END = IDX_ANIM_END - 3;
	for (int i = IDX_ANIM_START; i < IDX_ANIM_END; i++)
	{
		indexes.push_back(i);
		if (i >= IDX_LONG_START && i < IDX_LONG_END)
		{
			times.push_back(1.f);
		}
		else
		{
			times.push_back(0.3f);
		}
		
	}
	SpriteRenderer->CreateAnimation("Jump", SPRITE_NAME, indexes, times, false);
}

bool AMushroom::IsJump()
{
	bool isJump = Random.RandomInt(0, 5) == 0;	// ~16%
	EMonsterState nowState = static_cast<EMonsterState>(Fsm.GetState());

	if (isJump && nowState != EMonsterState::JUMPING)
	{
		return true;
	}

	return false;
}

/* FSM callbacks */
void AMushroom::Thinking(float _deltaTime)
{
	//OutputDebugString(("PathFinderIdx: " + std::to_string(PathFinderIdx) + "\n").c_str());
	if (Route.empty())
	{
		FindPath();

		if (IsJump())
		{
			Fsm.ChangeState(EMonsterState::JUMPING);
		}
	}
	else
	{
		Fsm.ChangeState(EMonsterState::WALKING);
	}
}

void AMushroom::Jumping(float _deltaTime)
{
	SpriteRenderer->ChangeAnimation("Jump");

	if (SpriteRenderer->IsCurAnimationEnd())
	{
		Fsm.ChangeState(EMonsterState::THINKING);
	}
}