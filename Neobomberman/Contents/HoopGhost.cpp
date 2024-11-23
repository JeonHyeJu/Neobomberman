#include "PreCompile.h"
#include "ContentsEnum.h"
#include "HoopGhost.h"
#include "UtilFn.h"
#include <EngineCore/SpriteRenderer.h>
#include <algorithm>

AHoopGhost::AHoopGhost()
: AMonster()
{
	SetName("HoopGhost");

	Fsm.ChangeStateFunction(EMonsterState::INIT_BLINK, nullptr);	// Off the initial blinking
	Fsm.ChangeStateFunction(EMonsterState::INIT_WALKING, nullptr);	// Off the initial walking
	Fsm.ChangeStateFunction(EMonsterState::THINKING, nullptr);
	Fsm.ChangeStateFunction(EMonsterState::DYING, nullptr);
	Fsm.ChangeStateFunction(EMonsterState::PASS_AWAY, nullptr);

	Fsm.CreateState(EMonsterState::PRESS_DOWN, std::bind(&AHoopGhost::PressingDown, this, std::placeholders::_1));

	InitMoveEllipse();
}

AHoopGhost::~AHoopGhost()
{

}

void AHoopGhost::BeginPlay()
{
	AMonster::BeginPlay();
	
	CenterLoc = GetActorLocation();
	Fsm.ChangeState(EMonsterState::WALKING);
}

void AHoopGhost::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);
}

void AHoopGhost::InitSprite()
{
	FVector2D size{ 256, 256 };
	SRBody = CreateDefaultSubObject<USpriteRenderer>();
	SRBody->SetSprite(SPRITE_NAME);
	SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SRBody->SetComponentScale(size);
	SRBody->SetPivotType(PivotType::Bot);
	SRBody->SetOrder(ERenderOrder::BOSS);

	const float animSpeed = .25f;
	{
		const int MAX_FRAME = 4;
		std::vector<int> idxs;
		std::vector<float> times;

		idxs.resize(MAX_FRAME);
		times.resize(MAX_FRAME);

		for (int i = 0; i < MAX_FRAME; i++)
		{
			idxs.push_back(i);
			if (i < MAX_FRAME - 1)
			{
				times.push_back(animSpeed);
			}
			else
			{
				times.push_back(animSpeed * 4);
			}
		}

		SRBody->CreateAnimation(ANIM_START_HOOP, SPRITE_NAME, idxs, times, false);
	}

	SRBody->CreateAnimation(ANIM_RUN_HOOP, SPRITE_NAME, 6, 13, animSpeed*2.f);
	SRBody->CreateAnimation(ANIM_RUN_DOWN, SPRITE_NAME, 13, 19, animSpeed);

	SetScore(EMonsterScore::S800);

	IsInited = true;	// Temp
}

void AHoopGhost::InitMoveEllipse()
{
	// It was already reserved in function.
	EllipseXPts = UtilFn::LoadIntVecStr("Resources\\Data", "ellipse_x_pts.txt");	// Temp
	EllipseYPts = UtilFn::LoadIntVecStr("Resources\\Data", "ellipse_y_pts.txt");	// Temp
	EllipsePtrSize = static_cast<int>(EllipseYPts.size());

	if (EllipseXPts.size() != EllipseYPts.size())
	{
		MSGASSERT("타원의 X, Y 사이즈가 다릅니다.");
	}

	InitRoundingIdx = static_cast<int>(EllipsePtrSize * .25f);
	RoundingIdx = InitRoundingIdx;
}

void AHoopGhost::ChangeMoveAnim(const FVector2D& _direction)
{
	SRBody->ChangeAnimation(ANIM_RUN_HOOP);
}

void AHoopGhost::OnWalk()
{
	SRBody->ChangeAnimation(ANIM_START_HOOP);
}

void AHoopGhost::Walking(float _deltaTime)
{
	static float elapsedSesc = 0.f;

	elapsedSesc += _deltaTime;

	if (SRBody->IsCurAnimationEnd())	// START_HOOP
	{
		SRBody->ChangeAnimation(ANIM_RUN_HOOP);
	}

	if (elapsedSesc >= .05f)
	{
		elapsedSesc = 0.f;

		if (SRBody->GetCurAnimName() == ANIM_RUN_HOOP)
		{
			if (EllipsePtrSize < RoundingIdx) return;

			int x = EllipseXPts[RoundingIdx];
			int y = EllipseYPts[RoundingIdx];

			// TODO
			FVector2D move;
			CenterLoc += move;

			FVector2D loc = CenterLoc + FVector2D{ x, -y };
			RoundingIdx = (RoundingIdx + 1) % EllipsePtrSize;

			SetActorLocation(loc);
		}
	}
}

void AHoopGhost::PressingDown(float _deltaTime)
{
	RoundingIdx = InitRoundingIdx;
}