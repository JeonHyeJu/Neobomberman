#include "PreCompile.h"
#include "GlobalVar.h"
#include "ContentsEnum.h"
#include "HoopGhost.h"
#include "Player.h"
#include "GameData.h"
#include "UtilFn.h"
#include "GameUI.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/2DCollision.h>
#include <EnginePlatform/EngineSound.h>
#include <algorithm>

AHoopGhost::AHoopGhost()
: AMonster()
{
	SetName("HoopGhost");
	SetCanHit(false);
	Properties.Health = MAX_HEALTH;
	SetScore(EMonsterScore::S1600);
	SetDamageSize(GlobalVar::BOMB_SIZE * 4);
	SetDamageMargin(URect(0, 30, 40, 40));

	InitMoveEllipse();

	Fsm.CreateState(EMonsterState::WAIT_START_DELAY, std::bind(&AHoopGhost::WaitingStartDelay, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::WALKING, std::bind(&AHoopGhost::Walking, this, std::placeholders::_1), std::bind(&AHoopGhost::OnWalk, this));
	Fsm.CreateState(EMonsterState::PRESS_DOWN, std::bind(&AHoopGhost::PressingDown, this, std::placeholders::_1), std::bind(&AHoopGhost::OnPressDown, this));
	Fsm.CreateState(EMonsterState::DAMAGED, std::bind(&AHoopGhost::Damaging, this, std::placeholders::_1), std::bind(&AHoopGhost::OnDamage, this));
	Fsm.CreateState(EMonsterState::DYING, std::bind(&AHoopGhost::Dying, this, std::placeholders::_1));
	Fsm.CreateState(EMonsterState::PASS_AWAY, std::bind(&AHoopGhost::PassAwaing, this, std::placeholders::_1), std::bind(&AHoopGhost::OnPassaway, this));

	Fsm.ChangeState(EMonsterState::WAIT_START_DELAY);
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
	DelaySecs = WaitDelay;
}

void AHoopGhost::Tick(float _deltaTime)
{
	AMonster::Tick(_deltaTime);

	Fsm.Update(_deltaTime);
}

void AHoopGhost::InitSprite()
{
	FVector2D size = GlobalVar::HOOPGHOST_SIZE;
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

		SRBody->CreateAnimation(ANIM_DAMAGED, SPRITE_NAME, idxs, times, false, 5);
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

	SRBody->CreateAnimation(ANIM_AFTER_PRESS, SPRITE_NAME, { 16, 17, 18, 19 }, { .25f, .25f, .25f, .25f }, false);	// Temp

	{
		const char* SHADOW_S_SPRITE_NAME = "HoopGhostShadow_S.png";
		const char* SHADOW_L_SPRITE_NAME = "HoopGhostShadow_L.png";

		FVector2D size{ 128, 64 };
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
		const char* SPRITE_DESTROY_PATH = "HoopGhostCloud";

		FVector2D size = GlobalVar::HOOPGHOST_SIZE;
		SRCloud = CreateDefaultSubObject<USpriteRenderer>();
		SRCloud->SetSprite(SPRITE_DESTROY_PATH);
		SRCloud->SetComponentLocation(size.Half().Half() - FVector2D{ 0.f, 32.f });
		SRCloud->SetComponentScale(size);
		SRCloud->CreateAnimation(ANIM_DESTROY, SPRITE_DESTROY_PATH, 0, 189, .025f, false);
		SRCloud->SetOrder(ERenderOrder::BOSS_CLOUD);
		SRCloud->SetActive(false);
	}
	

	PadToBottomSize = SRBody->GetComponentScale().Y * .4f;
	MoveSize = round(PadToBottomSize * .5f);

	//DebugOn();
}

void AHoopGhost::InitCollision()
{
	FVector2D colSize = DetectShape;
	FVector2D shadowSize = SRShadowL->GetComponentScale();
	float marginX = std::fabs(shadowSize.X - DetectShape.X) * .5f;

	Collision = CreateDefaultSubObject<U2DCollision>();
	Collision->SetComponentLocation(SRShadowL->GetComponentLocation() - FVector2D{ -marginX, 8.f });
	Collision->SetComponentScale(colSize);
	Collision->SetCollisionGroup(ECollisionGroup::MonsterBody);
	Collision->SetCollisionType(ECollisionType::Rect);
	Collision->SetActive(false);
}

void AHoopGhost::InitMoveEllipse()
{
	GlobalPath path;
	const char* ELLIPSE_X_TXT = "ellipse_x_pts.txt";
	const char* ELLIPSE_Y_TXT = "ellipse_y_pts.txt";

	// It was already reserved in function.
	EllipseXPts = UtilFn::LoadIntVecStr(path.GetDataPath(), ELLIPSE_X_TXT);
	EllipseYPts = UtilFn::LoadIntVecStr(path.GetDataPath(), ELLIPSE_Y_TXT);
	if (EllipseXPts.size() != EllipseYPts.size())
	{
		MSGASSERT("타원의 X, Y 사이즈가 다릅니다.");
	}

	EllipsePtSize = static_cast<int>(EllipseYPts.size());
	int initRoundingIdx = static_cast<int>(EllipsePtSize * .25f);

	RoundingIdx = initRoundingIdx;
}

void AHoopGhost::Damaged(unsigned __int8 _power)
{
	if (static_cast<EMonsterState>(Fsm.GetState()) != EMonsterState::DAMAGED)
	{
		Properties.Health -= _power;
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

bool AHoopGhost::IsDying()
{
	return (static_cast<EMonsterState>(Fsm.GetState()) == EMonsterState::DYING);
}

void AHoopGhost::ChangeMoveAnim(const FVector2D& _direction)
{
	SRBody->ChangeAnimation(ANIM_RUN_HOOP);
}

void AHoopGhost::toggleShadow()
{
	if (!SRShadowS || !SRShadowL) return;
	SRShadowS->SetActive(!SRShadowS->IsActive());
	SRShadowL->SetActive(!SRShadowL->IsActive());
}

void AHoopGhost::GoingUp()
{
	Collision->SetActive(false);
	SetCanHit(false);

	if (GetActorLocation().Y > SavedLoc.Y)
	{
		AddActorLocation(FVector2D{ 0.f, -MoveSize });
		//OutputDebugString(("Up: " + std::to_string(Collision->GetComponentLocation().Y) + " -> ").c_str());
		Collision->AddComponentLocation(FVector2D{ 0.f, MoveSize });
		SRShadowL->AddComponentLocation(FVector2D{ 0.f, MoveSize });
		//OutputDebugString((std::to_string(Collision->GetComponentLocation().Y) + "\n").c_str());
	}
	else
	{
		DelaySecs = WaitDelay;
		Fsm.ChangeState(EMonsterState::WALKING);
	}
}

void AHoopGhost::SetSpeed2x()
{
	WaitDelay *= .5f;
	WalkingDelay -= .01f;

	const int MAX_FRAME = 5;
	std::vector<float> times(MAX_FRAME, .25f);
	times[1] = 1.f;
	SRBody->ChangeAnimFrameTime(ANIM_RUN_PRESS, SPRITE_NAME, times);
}

/* Fsm start function */
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
	if (Properties.Health == 2)
	{
		SetSpeed2x();
	}
	SRBody->ChangeAnimation(ANIM_DAMAGED);
}

void AHoopGhost::OnPassaway()
{
	SRBody->SetActive(false);
	SRShadowS->SetActive(false);
	SRShadowL->SetActive(false);
	SRCloud->SetActive(false);

	GameData::GetInstance().AddPlayer1Score(GetScore());
}

/* Fsm update function */
void AHoopGhost::WaitingStartDelay(float _deltaTime)
{
	ElapsedSesc += _deltaTime;
	if (ElapsedSesc > StartDelayMs)
	{
		ElapsedSesc = 0.f;
		Fsm.ChangeState(EMonsterState::WALKING);
	}
}

void AHoopGhost::Walking(float _deltaTime)
{
	if (SRBody->GetCurAnimName() == ANIM_START_HOOP)
	{
		if (SRBody->IsCurAnimationEnd())
		{
			SRBody->ChangeAnimation(ANIM_RUN_HOOP);
		}
		return;
	}

	ElapsedSesc += _deltaTime;
	if (ElapsedSesc >= WalkingDelay)
	{
		ElapsedSesc = 0.f;

		if (EllipsePtSize < RoundingIdx) return;

		UEngineSound::Play(SFXHoop);

		int x = EllipseXPts[RoundingIdx];
		int y = EllipseYPts[RoundingIdx];

		// TODO
		FVector2D move;
		CenterLoc += move;

		FVector2D loc = CenterLoc + FVector2D{ x, -y };
		RoundingIdx = (RoundingIdx + 1) % EllipsePtSize;

		SetActorLocation(loc);

		// Press down
		if (DelaySecs-- <= 0)
		{
			SavedLoc = loc;

			BottomLoc = loc + FVector2D{ DetectShape.hX(), PadToBottomSize + SHADOW_MARGIN };
			URect rect(
				BottomLoc.X - DetectShape.hX(),
				BottomLoc.X + DetectShape.hX(),
				BottomLoc.Y - DetectShape.hX(),
				BottomLoc.Y + DetectShape.hX()
			);

			APlayer* player = GetWorld()->GetPawn<APlayer>();
			FVector2D playerLoc = player->GetActorLocation();

			if (playerLoc.X >= rect.Left && playerLoc.X <= rect.Right
				&& playerLoc.Y <= rect.Bottom && playerLoc.Y >= rect.Top)
			{
				Fsm.ChangeState(EMonsterState::PRESS_DOWN);
			}
		}
	}
}

void AHoopGhost::Damaging(float _deltaTime)
{
	ElapsedSesc += _deltaTime;
	if (ElapsedSesc > .1f)
	{
		ElapsedSesc = 0.f;

		if (!SRBody->IsCurAnimationEnd()) return;

		const std::string& curAnimName = SRBody->GetCurAnimName();
		if (curAnimName == ANIM_DAMAGED)
		{
			if (Properties.Health <= 0)
			{
				Kill();
			}
			else
			{
				SRBody->ChangeAnimation(ANIM_AFTER_PRESS);
			}
		}
		else if (curAnimName == ANIM_AFTER_PRESS)
		{
			GoingUp();
		}
	}
}

void AHoopGhost::PressingDown(float _deltaTime)
{
	ElapsedSesc += _deltaTime;
	if (ElapsedSesc > .1f)
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

		if (SRBody->GetCurAnimName() == ANIM_RUN_PRESS)
		{
			if (isAnimEnd)
			{
				GoingUp();
			}
		}
		else
		{
			if (GetActorLocation().Y < BottomLoc.Y - BodyHHY)
			{
				AddActorLocation(FVector2D{ 0.f, MoveSize });
				//OutputDebugString(("Down: " + std::to_string(Collision->GetComponentLocation().Y) + " -> ").c_str());
				Collision->AddComponentLocation(FVector2D{ 0.f, -MoveSize });
				SRShadowL->AddComponentLocation(FVector2D{ 0.f, -MoveSize });
				//OutputDebugString((std::to_string(Collision->GetComponentLocation().Y) + "\n").c_str());
			}
			else
			{
				Collision->SetActive(true);
				SetCanHit(true);

				SRBody->ChangeAnimation(ANIM_RUN_PRESS);
			}
		}
	}
}

void AHoopGhost::Dying(float _deltaTime)
{
	ElapsedSesc += _deltaTime;
	if (ElapsedSesc > .5f)
	{
		ElapsedSesc = 0.f;

		if (SRCloud->IsCurAnimationEnd())
		{
			Fsm.ChangeState(EMonsterState::PASS_AWAY);
			return;
		}
	}

	ExplosionSecs += _deltaTime;
	if (ExplosionSecs > .15f)
	{
		ExplosionSecs = 0.f;
		UEngineSound::Play(SFXCrash, -1, 0, false);
	}

	BlinkElapsedSecs += _deltaTime;
	if (BlinkElapsedSecs > 0.05f)
	{
		BlinkElapsedSecs = 0.f;
		SRBody->SetActiveSwitch();
	}
}

void AHoopGhost::PassAwaing(float _deltaTime)
{
	if (GetIsDestroiable()) return;

	SetIsDestroiable(true);
}