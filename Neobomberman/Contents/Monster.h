#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/PathFindAStar.h>
#include <EngineBase/FSMStateManager.h>

enum class EMonsterState
{
	INIT_BLINK = 0,
	INIT_WALKING,
	THINKING,
	WALKING,
	JUMPING,
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

	virtual void Kill() = 0;

	virtual void OnPause() override;
	virtual void OnResume() override;

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

	static int MonsterCount;

protected:
	FVector2D GetDirection(const FIntPoint& _vec);
	void SetScore(EMonsterScore _score);
	bool IsRouteEmpty();
	bool IsArrivedAtOneBlock();
	void FindPath();

	ABaseMap* CurMap = nullptr;

	EMonsterScore Score = EMonsterScore::S100;
	float Speed = 1.f;

	const int MonsterIdx;
	bool IsDestroiable = false;

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
	
private:
	/* Path finder */
	static UPathFindAStar PathFinder;
};
