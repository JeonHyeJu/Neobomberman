#include "PreCompile.h"
#include "ContentsEnum.h"
#include "HoopGhost.h"
#include "Player.h"
#include "UtilFn.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/2DCollision.h>
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

	Fsm.CreateState(EMonsterState::PRESS_DOWN, std::bind(&AHoopGhost::PressingDown, this, std::placeholders::_1), std::bind(&AHoopGhost::OnPressDown, this));

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

	// for debug start
	{
		FVector2D size{ DetectSizeHalf * 2, DetectSizeHalf * 2 };
		DebugRenderer = CreateDefaultSubObject<USpriteRenderer>();
		DebugRenderer->SetSprite(DEBUG_IMG_PATH);
		DebugRenderer->SetComponentLocation({ size.hX(), size.hY() });
		DebugRenderer->SetComponentScale(size);
		DebugRenderer->SetOrder(static_cast<int>(ERenderOrder::BOSS)+1);
	}
	// for debug end

	const float animSpeed = .25f;
	{
		const int MAX_FRAME = 4;
		std::vector<int> idxs;
		std::vector<float> times(MAX_FRAME, animSpeed);

		idxs.reserve(MAX_FRAME);

		for (int i = 0; i < MAX_FRAME; i++)
		{
			idxs.push_back(i);
			if (i >= MAX_FRAME- 1)
			{
				times[i] = animSpeed * 4;
			}
		}

		SRBody->CreateAnimation(ANIM_START_HOOP, SPRITE_NAME, idxs, times, false);
	}

	SRBody->CreateAnimation(ANIM_RUN_HOOP, SPRITE_NAME, 6, 13, animSpeed*2.f);
	SRBody->CreateAnimation(ANIM_START_PRESS, SPRITE_NAME, 13, 14, animSpeed*3.f, false);

	{
		const int MAX_FRAME = 5;
		std::vector<int> idxs;
		std::vector<float> times(MAX_FRAME, animSpeed);
		idxs.reserve(MAX_FRAME);
		times[1] = 2.f;
		for (int i = 0; i < MAX_FRAME; i++)
		{
			idxs.push_back(i+15);
		}

		SRBody->CreateAnimation(ANIM_RUN_PRESS, SPRITE_NAME, idxs, times, false);
	}

	SetScore(EMonsterScore::S800);

	PadToBottomSize = SRBody->GetComponentScale().hY();

	// Temp. Must be fixed!
	IsInited = true;	// Temp

	// Move to shadow actor
	FVector2D colSize{ DetectSizeHalf * 2.f, DetectSizeHalf * 2.f };
	Collision->SetComponentLocation({ DetectSizeHalf, PadToBottomSize});
	Collision->SetComponentScale(colSize);
	Collision->SetActive(false);

	DebugOn();
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

void AHoopGhost::OnPressDown()
{
	SRBody->ChangeAnimation(ANIM_START_PRESS);
}

void AHoopGhost::Walking(float _deltaTime)
{
	ElapsedSesc += _deltaTime;

	if (SRBody->IsCurAnimationEnd())	// START_HOOP
	{
		SRBody->ChangeAnimation(ANIM_RUN_HOOP);
	}

	if (ElapsedSesc >= WALKING_DELAY)
	{
		ElapsedSesc = 0.f;

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
			SavedLoc = loc;

			// Temp
			BottomLoc = loc + FVector2D{ DetectSizeHalf, PadToBottomSize };	// TODO: reset
			URect rect(
				BottomLoc.X - DetectSizeHalf,
				BottomLoc.X + DetectSizeHalf,
				BottomLoc.Y - DetectSizeHalf,
				BottomLoc.Y + DetectSizeHalf
			);

			APlayer* player = GetWorld()->GetPawn<APlayer>();
			FVector2D playerLoc = player->GetActorLocation();

			DebugRenderer->SetComponentLocation(BottomLoc - loc);

			if (DelaySecs-- <= 0)
			{
				if (playerLoc.X >= rect.Left && playerLoc.X <= rect.Right
					&& playerLoc.Y <= rect.Bottom && playerLoc.Y >= rect.Top)
				{
					Fsm.ChangeState(EMonsterState::PRESS_DOWN);
				}
			}
		}
	}
}

void AHoopGhost::PressingDown(float _deltaTime)
{
	ElapsedSesc += _deltaTime;

	if (ElapsedSesc > .25f)
	{
		ElapsedSesc = 0.f;

		bool isAnimEnd = SRBody->IsCurAnimationEnd();
		if (SRBody->GetCurAnimName() == ANIM_START_PRESS)
		{
			if (!isAnimEnd)
			{
				return;
			}
		}

		static const float MOVE_SIZE = PadToBottomSize * .5f;
		if (SRBody->GetCurAnimName() == ANIM_RUN_PRESS)
		{
			if (isAnimEnd)
			{
				if (GetActorLocation().Y > SavedLoc.Y)
				{
					AddActorLocation(FVector2D{ 0.f, -MOVE_SIZE });
				}
				else
				{
					DelaySecs = WAIT_DELAY;
					Fsm.ChangeState(EMonsterState::WALKING);
				}
			}
		}
		else
		{
			if (GetActorLocation().Y < BottomLoc.Y)
			{
				AddActorLocation(FVector2D{ 0.f, MOVE_SIZE });
			}
			else
			{
				SRBody->ChangeAnimation(ANIM_RUN_PRESS);
				return;
			}
		}
	}
}