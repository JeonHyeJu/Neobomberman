#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/PathFindAStar.h>
#include <EngineBase/FSMStateManager.h>

enum class EMonsterBlink
{
	OFF = 0,
	ON
};

enum class EMonsterState
{
	WAIT_START_DELAY = 0,
	WAIT_INTIALIZE,
	INIT_WALKING,
	THINKING,
	WALKING,
	JUMPING,
	DAMAGED,
	PRESS_DOWN,
	DYING,
	PASS_AWAY,
};

enum class EMonsterScore
{
	S100 = 100,
	S200 = 200,
	S400 = 400,
	S800 = 800,
	S1600 = 1600,
};

class AMonster : public AActor
{
public:
	struct SProperties
	{
		EMonsterScore Score = EMonsterScore::S100;
		int Health = 1;
		float Speed = 50.f;
		bool CanHit = true;
		bool IsDestroiable = false;
		FVector2D DamageSize;
		URect DamageMargin = URect{ 0, 0, 0, 0 };
	};

	AMonster();
	virtual ~AMonster() {}

	AMonster(const AMonster& _other) = delete;
	AMonster(AMonster&& _other) noexcept = delete;
	AMonster& operator=(const AMonster& _other) = delete;
	AMonster& operator=(AMonster&& _other) noexcept = delete;

	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;

	virtual void Damaged(unsigned __int8 _power=1) = 0;
	virtual void Kill() = 0;

	virtual void OnPause() override;
	virtual void OnResume() override;

	// Temp. for boss
	virtual bool IsDying() { return false; };

	/* Setter */
	void SetCurMap(class ABaseMap* _map);
	void SetFirstDestination(const FIntPoint& _idx)
	{
		FirstIdx = _idx;
	}
	void SetStartDelay(float _seconds)
	{
		StartDelayMs = _seconds;
	}

	/* Getter */
	inline bool GetCanHit() const
	{
		return Properties.CanHit;
	}
	inline int GetScore() const
	{
		return static_cast<int>(Properties.Score);
	}
	inline bool GetIsDestroiable() const
	{
		return Properties.IsDestroiable;
	}
	inline const FVector2D& GetDamageSize() const
	{
		return Properties.DamageSize;
	}
	inline const URect& GetDamageMargin() const
	{
		return Properties.DamageMargin;
	}

	static int MonsterCount;

protected:
	FVector2D GetDirection(const FIntPoint& _vec);
	bool IsRouteEmpty();
	bool IsArrivedAtOneBlock();
	void FindPath(bool _force=false);
	void SetScore(EMonsterScore _score)
	{
		Properties.Score = _score;
	}
	void SetCanHit(bool _val)
	{
		Properties.CanHit = _val;
	}
	void SetIsDestroiable(bool _val)
	{
		Properties.IsDestroiable = _val;
	}
	void SetDamageSize(const FVector2D& _val)
	{
		Properties.DamageSize = _val;
	}
	void SetDamageMargin(const URect& _val)
	{
		Properties.DamageMargin = _val;
	}
	void ClearRoute()
	{
		Route.clear();
	}

	ABaseMap* CurMapPtr = nullptr;

	const int MonsterIdx;

	const int SCORE_ANIM_CNT = 7;

	const float BLINK_SECONDS = 2.f;
	const float BLINK_DELAY = .05f;

	const char* CLOUD_SPRITE_PATH = "MonsterCloud.png";
	const char* MONSTER_SCORE_PATH = "MonsterScore.png";
	const char* SFXDying = "MonsterDying.mp3";

	/* Initialization info */
	float StartDelayMs = 0.f;
	FIntPoint FirstIdx;

	/* Path finder */
	int PathFinderIdx = 0;
	std::list<FIntPoint> Route;
	FIntPoint Destination = FIntPoint::NEGATIVE_ONE;
	FIntPoint PrevRouteIdx;

	SProperties Properties;
	
private:
	/* Path finder */
	static UPathFindAStar PathFinder;
};
