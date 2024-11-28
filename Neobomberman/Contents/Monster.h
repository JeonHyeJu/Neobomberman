#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/PathFindAStar.h>
#include <EngineBase/FSMStateManager.h>

enum class EMonsterState
{
	BLINKING = 0,
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
	virtual FVector2D GetMonsterSize() = 0;
	virtual FIntPoint GetDamageRange() = 0;

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
	inline int GetScore() const
	{
		return static_cast<int>(Score);
	}
	inline bool GetIsDestroiable() const
	{
		return IsDestroiable;
	}
	inline float GetStartDelay() const
	{
		return StartDelayMs;
	}
	bool GetCanHit()
	{
		return CanHit;
	}

	static int MonsterCount;

protected:
	FVector2D GetDirection(const FIntPoint& _vec);
	void SetScore(EMonsterScore _score);
	bool IsRouteEmpty();
	bool IsArrivedAtOneBlock();
	void FindPath(bool _force=false);
	void ClearRoute()
	{
		Route.clear();
	}

	ABaseMap* CurMapPtr = nullptr;

	EMonsterScore Score = EMonsterScore::S100;
	float Speed = 50.f;

	const int MonsterIdx;

	const int SCORE_ANIM_CNT = 7;
	const float BLINK_SECONDS = 2.f;

	const char* CLOUD_SPRITE_PATH = "MonsterCloud.png";
	const char* MONSTER_SCORE_PATH = "MonsterScore.png";

	float StartDelayMs = 0.f;

	FIntPoint FirstIdx;

	/* Path finder */
	int PathFinderIdx = 0;
	std::list<FIntPoint> Route;
	FIntPoint Destination = FIntPoint::NEGATIVE_ONE;
	FIntPoint PrevIdx;

	int Health = 1;

	bool IsDestroiable = false;
	bool CanHit = false;

	const char* SFXDying = "MonsterDying.mp3";
	
private:
	/* Path finder */
	static UPathFindAStar PathFinder;
};
