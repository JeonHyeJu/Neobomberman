#include "PreCompile.h"
#include "Mushroom.h"
#include "ContentsEnum.h"
#include "GlobalVar.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/PathFindAStar.h>

AMushroom::AMushroom()
{
	Speed = 20.f;
	Random.SetSeed(MonsterIdx);

	Fsm.CreateState(EMonsterState::JUMPING, std::bind(&AMushroom::Jumping, this, std::placeholders::_1));
	SetName("Mushroom");
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

void AMushroom::InitSprite()
{
	FVector2D size = GlobalVar::BOMBERMAN_SIZE;
	const float animTime = 0.5f;

	SRBody = CreateDefaultSubObject<USpriteRenderer>();
	SRBody->SetSprite(SPRITE_NAME);
	SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SRBody->SetComponentScale(size);
	SRBody->SetPivotType(PivotType::Bot);
	SRBody->SetOrder(ERenderOrder::MONSTER);

	SRBody->CreateAnimation(ANIM_IDLE_UP, SPRITE_NAME, 0, 0, animTime);
	SRBody->CreateAnimation(ANIM_IDLE_DOWN, SPRITE_NAME, 22, 22, animTime);
	SRBody->CreateAnimation(ANIM_IDLE_LEFT, SPRITE_NAME, 33, 33, animTime);
	SRBody->CreateAnimation(ANIM_IDLE_RIGHT, SPRITE_NAME, 11, 11, animTime);

	SRBody->CreateAnimation(ANIM_RUN_UP, SPRITE_NAME, 1, 6, animTime);
	SRBody->CreateAnimation(ANIM_RUN_DOWN, SPRITE_NAME, 23, 28, animTime);
	SRBody->CreateAnimation(ANIM_RUN_LEFT, SPRITE_NAME, 34, 39, animTime);
	SRBody->CreateAnimation(ANIM_RUN_RIGHT, SPRITE_NAME, 12, 17, animTime);

	std::vector<int> idxs;
	std::vector<float> times;

	const int IDX_ANIM_START = 44;
	const int IDX_ANIM_END = 54;
	int jumpAnimSize = IDX_ANIM_END - IDX_ANIM_START;

	idxs.reserve(jumpAnimSize);
	times.reserve(jumpAnimSize);

	const int IDX_LONG_START = IDX_ANIM_START + 3;
	const int IDX_LONG_END = IDX_ANIM_END - 3;
	for (int i = IDX_ANIM_START; i < IDX_ANIM_END; i++)
	{
		idxs.push_back(i);
		if (i >= IDX_LONG_START && i < IDX_LONG_END)
		{
			times.push_back(1.f);
		}
		else
		{
			times.push_back(0.3f);
		}
		
	}

	SRBody->CreateAnimation(ANIM_JUMP, SPRITE_NAME, idxs, times, false);
	SetScore(EMonsterScore::S100);

	SRBody->SetActive(false);
}

void AMushroom::ChangeMoveAnim(const FVector2D& _direction)
{
	if (_direction == FVector2D::UP)
	{
		SRBody->ChangeAnimation(ANIM_RUN_UP);
	}
	else if (_direction == FVector2D::DOWN)
	{
		SRBody->ChangeAnimation(ANIM_RUN_DOWN);
	}
	else if (_direction == FVector2D::LEFT)
	{
		SRBody->ChangeAnimation(ANIM_RUN_LEFT);
	}
	else if (_direction == FVector2D::RIGHT)
	{
		SRBody->ChangeAnimation(ANIM_RUN_RIGHT);
	}
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
	if (IsRouteEmpty())
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
	SRBody->ChangeAnimation(ANIM_JUMP);

	if (SRBody->IsCurAnimationEnd())
	{
		Fsm.ChangeState(EMonsterState::THINKING);
	}
}