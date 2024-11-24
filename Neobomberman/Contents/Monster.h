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

enum class EMonsterType
{
	NORMAL = 0,
	BOSS,
};

enum class EMonsterScore
{
	S100 = 100,
	S200 = 200,
	S400 = 400,
	S800 = 800,
};

class USpriteRenderer;
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
	virtual void InitSprite() = 0;
	virtual void InitCollision() = 0;
	virtual void ChangeMoveAnim(const FVector2D& _direction) = 0;

	virtual void OnPause() override;
	virtual void OnResume() override;

	FVector2D GetDirection(const FIntPoint& _vec);
	void Move(const FVector2D& direction, float _deltaTime);
	void Kill();

	/* Setter */
	void SetCurMap(class ABaseMap* _map);
	void SetFirstDestination(const FIntPoint& _idx);
	void SetStartDelay(float _seconds)
	{
		StartDelayMs = _seconds;
	}

	/* Getter */
	inline float GetStartDelay() const
	{
		return StartDelayMs;
	}
	inline int GetScore() const
	{
		return static_cast<int>(Score);
	}
	inline bool GetIsDetroiable() const
	{
		return IsDestroiable;
	}

	static int MonsterCount;

protected:
	void FindPath();
	bool IsArrivedAtOneBlock();
	bool IsRouteEmpty();
	void SetScore(EMonsterScore _score);

	/* FSM update callbacks */
	/* You don't need to call Super::function()! */
	virtual void Blinking(float _deltaTime);
	virtual void WalkingForStart(float _deltaTime);
	virtual void Thinking(float _deltaTime);
	virtual void Walking(float _deltaTime);
	virtual void Dying(float _deltaTime);
	virtual void PassAwaing(float _deltaTime);

	virtual void OnWalk() {}

	/* FSM start callbacks */
	virtual void OnPassaway();

	const char* MONSTER_SCORE_PATH = "MonsterScore.png";

	USpriteRenderer* SRBody = nullptr;
	ABaseMap* CurMap = nullptr;

	float Speed = 1.f;
	const int MonsterIdx;
	EMonsterType MonsterType = EMonsterType::NORMAL;
	
	UFSMStateManager Fsm;
	bool IsInited = false;

	class U2DCollision* Collision = nullptr;

private:
	const int SCORE_ANIM_CNT = 7;
	const float BLINK_SECONDS = 2.f;
	const char* CLOUD_SPRITE_PATH = "MonsterCloud.png";

	EMonsterScore Score = EMonsterScore::S100;

	bool IsDestroiable = false;

	float StartDelayMs = 0.f;
	FIntPoint FirstIdx;

	USpriteRenderer* SRCloud = nullptr;
	USpriteRenderer* SRScore = nullptr;

	/* Path finder */
	int PathFinderIdx = 0;
	static UPathFindAStar PathFinder;
	std::list<FIntPoint> Route;
	FIntPoint Destination = FIntPoint::NEGATIVE_ONE;
};
