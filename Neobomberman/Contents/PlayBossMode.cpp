#include "PreCompile.h"
#include "PlayBossMode.h"
#include "GameUI.h"
#include "BossMap.h"
#include "HoopGhost.h"
#include "Monster.h"
#include "Player.h"
#include "Result.h"
#include "GameData.h"
#include "StageTitle.h"
#include "Fade.h"

#include <EngineCore/Level.h>
#include <EnginePlatform/EngineInput.h>
#include <EnginePlatform/EngineSound.h>

APlayBossMode::APlayBossMode()
{
}

APlayBossMode::~APlayBossMode()
{
}

void APlayBossMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	AGameUI* gameUI = pLevel->SpawnActor<AGameUI>();

	Player = pLevel->GetPawn<APlayer>();
	Player->SetGameUI(gameUI);

	/* Stage 1-1 */
	ABossMap* pBossMap = pLevel->SpawnActor<ABossMap>();
	pBossMap->Initialize();
	pBossMap->BindExplodeEvent(std::bind(&APlayBossMode::OnExplodeBomb, this));
	CurMapPtr = pBossMap;

	Player->SetCurMap(pBossMap);
	Player->SetStartLoc(pBossMap->MatrixIdxToLocation(StartPoint));

	AHoopGhost* boss = pLevel->SpawnActor<AHoopGhost>();
	boss->SetCurMap(pBossMap);
	boss->SetFirstDestination({ 0, 0 });
	boss->SetActorLocation({ 256, 126 });	// Temp
	MonsterList.push_back(boss);

	AStageTitle* stageTitle = pLevel->SpawnActor<AStageTitle>();
	stageTitle->SetSubStage(2);

	AFade* fade = GetWorld()->SpawnActor<AFade>();
	fade->FadeIn();

	gameUI->StartTimer();

	UEngineSound::Play(SFXBg, -1, 100);
}

void APlayBossMode::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	static float elpasedSecs = 0.f;	// Temp
	elpasedSecs += _deltaTime;

	CheckCheat();

	if (elpasedSecs >= 1.f)
	{
		elpasedSecs = 0.f;
		CheckDeadMonster();
		if (IsAllMonstersDead())
		{
			if (!isShowingResult)
			{
				Player->ShowWinnerPose();
				isShowingResult = true;
				FadeOut();
			}
		}
	}
	/*if (elpasedSecs > .5f)
	{
		elpasedSecs = 0.f;

		if (!isShowingResult && Player->GetIsClear())
		{
			isShowingResult = true;
			FadeOut();
		}
	}*/
}

void APlayBossMode::CheckDeadMonster()
{
	std::list<AMonster*>::iterator it = MonsterList.begin();
	std::list<AMonster*>::iterator itEnd = MonsterList.end();
	for (; it != itEnd; ++it)
	{
		AMonster* monster = *it;

		if (monster->IsDying())
		{
			Player->BlockMove();
		}

		if (monster->GetIsDestroiable())
		{
			monster->Destroy();
			it = MonsterList.erase(it);
			if (it == itEnd) break;
		}
	}
}

void APlayBossMode::FadeOut()
{
	AFade::MainFade->BindEndEvent(std::bind(&APlayBossMode::OnEndFadeOut, this));
	AFade::MainFade->SetFadeMinMax(0.f, .5f);
	AFade::MainFade->SetFadeSpeed(.5f);
	AFade::MainFade->FadeOut();
}

void APlayBossMode::OnEndFadeOut()
{
	ResultScene = GetWorld()->SpawnActor<AResult>();
	ResultScene->SetNextLevel("Ending");
	ResultScene->ChangeRImage("ResultImage2.png", 0);

	int lastTime = AGameUI::GetLastTime();
	ResultScene->SetLastSecs(lastTime);
	ResultScene->SetTotal(GameData::GetInstance().GetPlayer1Score());
}

void APlayBossMode::OnExplodeBomb()
{
	if (CurMapPtr == nullptr) return;

	const std::vector<FIntPoint>& vec = CurMapPtr->GetSplashTileIdxs();

	std::list<AMonster*>::iterator it = MonsterList.begin();
	std::list<AMonster*>::iterator itEnd = MonsterList.end();
	for (; it != itEnd; ++it)
	{
		AMonster* monster = *it;
		if (monster->GetCanHit())
		{
			FIntPoint rangeIdx = monster->GetDamageRange();

			if (rangeIdx.X == 1 && rangeIdx.Y == 1)
			{
				FIntPoint curIdx = CurMapPtr->LocationToMatrixIdx(monster->GetActorLocation());
				if (CurMapPtr->IsInSplash(curIdx))
				{
					monster->Damaged();
				}
				return;
			}

			// I had to subtract from left top base.
			FVector2D monsterSize = monster->GetMonsterSize().Half().Half();
			FVector2D monsterLoc =monster->GetActorLocation();

			FIntPoint orgIdx = CurMapPtr->LocationToMatrixIdx(monster->GetActorLocation());
			FIntPoint curIdx = CurMapPtr->LocationToMatrixIdx(monster->GetActorLocation() + monsterSize);

			/*OutputDebugString(("monsterLoc: " + std::to_string(monsterLoc.X) + ", " + std::to_string(monsterLoc.Y) + "\n").c_str());
			OutputDebugString(("orgIdx: " + std::to_string(orgIdx.X) + ", " + std::to_string(orgIdx.Y) + "\n").c_str());
			OutputDebugString(("curIdx: " + std::to_string(curIdx.X) + ", " + std::to_string(curIdx.Y) + "\n").c_str());*/

			std::vector<FIntPoint> temp;	// TODO: check duplicated value
			for (int y = -rangeIdx.Y + 1; y < rangeIdx.Y; ++y)
			{
				for (int x = -rangeIdx.X + 1; x < rangeIdx.X; ++x)
				{
					temp.push_back(curIdx + FIntPoint( x, y ));
				}
			}

			//OutputDebugString("---------------------------- S\n");
			for (size_t i = 0, size = temp.size(); i < size; ++i)
			{
				//OutputDebugString(("checkIdx: " + std::to_string(temp[i].X) + ", " + std::to_string(temp[i].Y) + "\n").c_str());
				if (CurMapPtr->IsInSplash(temp[i]))
				{
					monster->Damaged();
				}
			}
			
			//OutputDebugString("---------------------------- E\n");
		}
	}

	// Check player
	FIntPoint playerIdx = CurMapPtr->LocationToMatrixIdx(Player->GetActorLocation());
	if (CurMapPtr->IsInSplash(playerIdx))
	{
		Player->Kill();
	}
}

bool APlayBossMode::IsAllMonstersDead() const
{
	return MonsterList.size() == 0;
}

void APlayBossMode::CheckCheat()
{
	if (UEngineInput::GetInst().IsDown('N'))
	{
		if (!isShowingResult && !IsAllMonstersDead())
		{
			std::list<AMonster*>::iterator it = MonsterList.begin();
			std::list<AMonster*>::iterator itEnd = MonsterList.end();
			for (; it != itEnd; ++it)
			{
				(*it)->Kill();
			}
		}
	}
}