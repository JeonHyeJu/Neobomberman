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
	Health = MAX_HEALTH;
	CanHit = true;	// Temp

	Fsm.CreateState(EMonsterState::WALKING, std::bind(&AHoopGhost::Walking, this, std::placeholders::_1), std::bind(&AHoopGhost::OnWalk, this));
	Fsm.CreateState(EMonsterState::PRESS_DOWN, std::bind(&AHoopGhost::PressingDown, this, std::placeholders::_1), std::bind(&AHoopGhost::OnPressDown, this));
	Fsm.CreateState(EMonsterState::DAMAGED, std::bind(&AHoopGhost::Damaging, this, std::placeholders::_1), std::bind(&AHoopGhost::OnDamage, this));
	Fsm.CreateState(EMonsterState::DYING, std::bind(&AHoopGhost::Dying, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::PASS_AWAY, std::bind(&AHoopGhost::PassAwaing, this, std::placeholders::_1));

	InitMoveEllipse();
}

AHoopGhost::~AHoopGhost()
{
}

void AHoopGhost::BeginPlay()
{
	AMonster::BeginPlay();

	InitSprite();
	InitCollision();

	CenterLoc = GetActorLocation();
	Fsm.ChangeState(EMonsterState::WALKING);
}

void AHoopGhost::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);

	Fsm.Update(_deltaTime);
}

void AHoopGhost::InitSprite()
{
	FVector2D size = MONSTER_SIZE;
	SRBody = CreateDefaultSubObject<USpriteRenderer>();
	SRBody->SetSprite(SPRITE_NAME);
	SRBody->SetComponentLocation({ size.X * .25f, size.Y * .5f });
	SRBody->SetComponentScale(size);
	SRBody->SetPivotType(PivotType::Bot);
	SRBody->SetOrder(ERenderOrder::BOSS);
	BodyHHY = size.Half().Half().Y;

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
		const int MAX_FRAME = 8;
		const int INVERT_IDX = 4;
		std::vector<int> idxs;
		std::vector<float> times(MAX_FRAME, .05f);

		idxs.reserve(MAX_FRAME);
		const int START_IDX = 20;

		for (int i = 0; i < MAX_FRAME; i++)
		{
			int idx = START_IDX;
			if (i > INVERT_IDX)
			{
				idx = idx + MAX_FRAME - i;
			}
			else
			{
				idx = idx + i;
			}

			idxs.push_back(idx);
		}

		SRBody->CreateAnimation(ANIM_DAMAGED, SPRITE_NAME, idxs, times, false, 3);
		SRBody->CreateAnimation(ANIM_DYING, SPRITE_NAME, 20, 20, 1.f, true);
	}

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

	{
		FVector2D size{ 128, 64 };	// Temp
		SRShadowS = CreateDefaultSubObject<USpriteRenderer>();
		SRShadowS->SetSprite(SHADOW_S_SPRITE_NAME);
		SRShadowS->SetComponentLocation({ size.hX() - 16.f, SRBody->GetComponentLocation().Y + SHADOW_MARGIN });
		SRShadowS->SetComponentScale(size);
		SRShadowS->SetOrder(ERenderOrder::BOSS_SHADOW);
		SRShadowS->SetAlphafloat(0.5f);
		SRShadowS->SetActive(false);

		SRShadowL = CreateDefaultSubObject<USpriteRenderer>();
		SRShadowL->SetSprite(SHADOW_L_SPRITE_NAME);
		SRShadowL->SetComponentLocation({ size.hX() - 16.f, SRBody->GetComponentLocation().Y + SHADOW_MARGIN });
		SRShadowL->SetComponentScale(size);
		SRShadowL->SetOrder(ERenderOrder::BOSS_SHADOW);
		SRShadowL->SetAlphafloat(0.5f);
		SRShadowL->SetActive(true);
	}

	{
		FVector2D size{ 256, 256 };
		SRCloud = CreateDefaultSubObject<USpriteRenderer>();
		SRCloud->SetSprite(DETROY_SPRITE_PATH);
		SRCloud->SetComponentLocation(size.Half().Half() - FVector2D{ 0.f, 32.f });
		SRCloud->SetComponentScale(size);
		SRCloud->CreateAnimation(ANIM_DESTROY, DETROY_SPRITE_PATH, 0, 189, .05f, false);
		SRCloud->SetOrder(ERenderOrder::BOSS_CLOUD);
		SRCloud->SetActive(false);
	}

	// for debug start
	/*{
		FVector2D size = DetectShape;
		DebugRenderer = CreateDefaultSubObject<USpriteRenderer>();
		DebugRenderer->SetSprite(DEBUG_IMG_PATH);
		DebugRenderer->SetComponentLocation({ size.hX(), size.hY() });
		DebugRenderer->SetComponentScale(size);
		DebugRenderer->SetOrder(static_cast<int>(ERenderOrder::BOSS) + 1);
		DebugRenderer->SetAlphafloat(.5f);
	}*/
	// for debug end
	

	SetScore(EMonsterScore::S1600);
	PadToBottomSize = SRBody->GetComponentScale().Y * .4f;

	DebugOn();
}

void AHoopGhost::InitCollision()
{
	FVector2D colSize = DetectShape;
	FVector2D shadowSize = SRShadowL->GetComponentScale();
	float marginX = std::fabs(shadowSize.X - DetectShape.X) * .5f;

	Collision = CreateDefaultSubObject<U2DCollision>();
	Collision->SetComponentLocation(SRShadowL->GetComponentLocation() - FVector2D{ marginX, 8.f });
	Collision->SetComponentScale(colSize);
	Collision->SetCollisionGroup(ECollisionGroup::MonsterBody);
	Collision->SetCollisionType(ECollisionType::Rect);
	Collision->SetActive(false);
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

FVector2D AHoopGhost::GetMonsterSize()
{
	return MONSTER_SIZE;
}

FIntPoint AHoopGhost::GetDamageRange()
{
	return FIntPoint{ 3, 2 };
}

void AHoopGhost::Damaged(unsigned __int8 _power)
{
	if (static_cast<EMonsterState>(Fsm.GetState()) != EMonsterState::DAMAGED)
	{
		Health -= _power;
		Fsm.ChangeState(EMonsterState::DAMAGED);
	}
}

void AHoopGhost::Kill()
{
	if (static_cast<EMonsterState>(Fsm.GetState()) != EMonsterState::DYING)
	{
		if (Collision != nullptr)
		{
			Collision->SetActive(false);
		}

		SRBody->ChangeAnimation(ANIM_DYING);
		SRCloud->ChangeAnimation(ANIM_DESTROY);
		SRCloud->SetActive(true);

		Fsm.ChangeState(EMonsterState::DYING);
	}
}

void AHoopGhost::ChangeMoveAnim(const FVector2D& _direction)
{
	SRBody->ChangeAnimation(ANIM_RUN_HOOP);
}

void AHoopGhost::OnWalk()
{
	SRBody->ChangeAnimation(ANIM_START_HOOP);
	toggleShadow();		// Shadow small on, Shadow large off
}

void AHoopGhost::OnPressDown()
{
	SRBody->ChangeAnimation(ANIM_START_PRESS);
	toggleShadow();		// Shadow small off, Shadow large on
}

void AHoopGhost::OnDamage()
{
	SRBody->ChangeAnimation(ANIM_DAMAGED);
}

void AHoopGhost::toggleShadow()
{
	if (!SRShadowS || !SRShadowL) return;
	SRShadowS->SetActive(!SRShadowS->IsActive());
	SRShadowL->SetActive(!SRShadowL->IsActive());
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
			SavedShadowLoc = SRShadowL->GetComponentLocation();

			// Temp
			BottomLoc = loc + FVector2D{ DetectShape.hX(), PadToBottomSize + SHADOW_MARGIN};	// TODO: reset
			URect rect(
				BottomLoc.X - DetectShape.hX(),
				BottomLoc.X + DetectShape.hX(),
				BottomLoc.Y - DetectShape.hX(),
				BottomLoc.Y + DetectShape.hX()
			);

			APlayer* player = GetWorld()->GetPawn<APlayer>();
			FVector2D playerLoc = player->GetActorLocation();

			//DebugRenderer->SetComponentLocation(BottomLoc - loc);

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

void AHoopGhost::Damaging(float _deltaTime)
{
	ElapsedSesc += _deltaTime;

	if (ElapsedSesc > .25f)
	{
		ElapsedSesc = 0.f;

		if (SRBody->GetCurAnimName() == ANIM_DAMAGED && SRBody->IsCurAnimationEnd())
		{
			if (Health <= 0)
			{
				Kill();
			}
			else
			{
				Fsm.ChangeState(EMonsterState::WALKING);
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
				Collision->SetActive(false);
				//CanHit = false;

				if (GetActorLocation().Y > SavedLoc.Y)
				{
					AddActorLocation(FVector2D{ 0.f, -MOVE_SIZE });
					Collision->AddComponentLocation(FVector2D{ 0.f, MOVE_SIZE });
					SRShadowL->AddComponentLocation(FVector2D{ 0.f, MOVE_SIZE });
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
			if (GetActorLocation().Y < BottomLoc.Y - BodyHHY)
			{
				AddActorLocation(FVector2D{ 0.f, MOVE_SIZE });
				Collision->AddComponentLocation(FVector2D{ 0.f, -MOVE_SIZE });
				SRShadowL->AddComponentLocation(FVector2D{ 0.f, -MOVE_SIZE });
			}
			else
			{
				FVector2D shadowSize = SRShadowL->GetComponentScale();
				//shadowSize.X -= 16.f * 2.f;		// left and right margin
				//shadowSize.Y -= 6.f * 2.f;		// top and bottom margin
				FVector2D shadowLoc = SRShadowL->GetComponentLocation();
				
				FVector2D leftTop{ shadowLoc.X - shadowSize.hX(), shadowLoc.Y - shadowSize.hY() };
				FVector2D rightTop{ shadowLoc.X + shadowSize.hX(), shadowLoc.Y - shadowSize.hY() };
				FVector2D leftBottom{ shadowLoc.X - shadowSize.hX(), shadowLoc.Y + shadowSize.hY() };
				FVector2D rightBottom{ shadowLoc.X + shadowSize.hX(), shadowLoc.Y + shadowSize.hY() };

				// Temp
				/*DebugRenderer->SetComponentLocation(shadowLoc);
				DebugRenderer->SetComponentScale(shadowSize);*/

				Collision->SetActive(true);
				CanHit = true;

				SRBody->ChangeAnimation(ANIM_RUN_PRESS);
				return;
			}
		}
	}
}

void AHoopGhost::Dying(float _deltaTime)
{
	static float blinkElapsedSecs = 0.f;

	ElapsedSesc += _deltaTime;

	if (ElapsedSesc > .5f)
	{
		ElapsedSesc = 0.f;

		if (SRCloud->IsCurAnimationEnd())
		{
			SRBody->SetActive(false);
			SRShadowS->SetActive(false);
			SRShadowL->SetActive(false);
			SRCloud->SetActive(false);
			Fsm.ChangeState(EMonsterState::PASS_AWAY);
			return;
		}
	}

	blinkElapsedSecs += _deltaTime;
	if (blinkElapsedSecs > 0.05f)
	{
		blinkElapsedSecs = 0.f;
		SRBody->SetActiveSwitch();
	}
}

void AHoopGhost::PassAwaing(float _deltaTime)
{
	if (IsDestroiable) return;

	IsDestroiable = true;
}