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
	DYING,
	PASS_AWAY,
};

enum class EMonsterScore
{
	S100 = 100,
	S200 = 200,
	S400 = 400,
};

class USpriteRenderer;
class AMonster : public AActor
{
public:
	AMonster();
	~AMonster();

	AMonster(const AMonster& _other) = delete;
	AMonster(AMonster&& _other) noexcept = delete;
	AMonster& operator=(const AMonster& _other) = delete;
	AMonster& operator=(AMonster&& _other) noexcept = delete;

	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;
	virtual void Init() = 0;

	virtual void OnPause() override;
	virtual void OnResume() override;

	void SetCurMap(class APlayMap* _map);
	void SetFirstDestination(const FIntPoint& _idx);
	void SetStartDelay(float _seconds)
	{
		StartDelayMs = _seconds;
	}
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
	bool IsArrivedAtOneBlock();
	FVector2D GetDirection(const FIntPoint& _vec);
	void Move(const FVector2D& direction, float _deltaTime);
	void Kill();

	static int MonsterCount;

protected:
	void FindPath();

	/* FSM callbacks */
	/* You don't need to call Super::function()! */
	virtual void Blinking(float _deltaTime);
	virtual void WalkingForStart(float _deltaTime);
	virtual void Thinking(float _deltaTime);
	virtual void Walking(float _deltaTime);
	virtual void Dying(float _deltaTime);
	virtual void PassAwaing(float _deltaTime);

	virtual void OnPassaway();
	void SetScore(EMonsterScore _score);

	const char* MONSTER_SCORE_PATH = "MonsterScore.png";

	USpriteRenderer* SRBody = nullptr;
	USpriteRenderer* SRScore = nullptr;
	class APlayMap* CurMap = nullptr;

	float Speed = 1.f;
	const float BLINK_SECONDS = 2.f;
	const int MonsterIdx;
	int PathFinderIdx = 0;

	float StartDelayMs = 0.f;
	bool IsInited = false;

	static UPathFindAStar PathFinder;
	std::list<FIntPoint> Route;
	FIntPoint Destination = FIntPoint::NEGATIVE_ONE;

	UFSMStateManager Fsm;
	FIntPoint FirstIdx;

	class U2DCollision* Collision = nullptr;

private:
	const char* CLOUD_SPRITE_PATH = "MonsterCloud.png";
	bool IsDestroiable = false;

	USpriteRenderer* SRCloud = nullptr;
	EMonsterScore Score = EMonsterScore::S100;
};
