#include "PreCompile.h"
#include "PlayGameMode.h"
#include "GameData.h"
#include "GameOver.h"
#include "GameUI.h"
#include "PlayMap.h"
#include "Player.h"
#include "Monster.h"
#include "Mushroom.h"
#include "Balloon.h"
#include "Result.h"
#include "Fade.h"

#include <EngineCore/Level.h>
#include <EngineCore/EngineAPICore.h>
#include <EnginePlatform/EngineInput.h>

APlayGameMode::APlayGameMode()
{
}

APlayGameMode::~APlayGameMode()
{
}

void APlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	GameUiPtr = pLevel->SpawnActor<AGameUI>();

	GameOverScenePtr = pLevel->SpawnActor<AGameOver>();
	GameOverScenePtr->SetActive(false);

	Player = pLevel->GetPawn<APlayer>();
	Player->SetGameUI(GameUiPtr);
	//Player->SetCollisionImage("Bg_1-Col.png");

	/* Stage 1-1 */
	APlayMap* pStage1 = pLevel->SpawnActor<APlayMap>();
	pStage1->SetPortalIdx(PORTAL_IDX_STAGE_1);
	pStage1->BindExplodeEvent(std::bind(&APlayGameMode::OnExplodeBomb, this));
	CurMapPtr = pStage1;

	pStage1->InitMap();

	Player->SetCurMap(pStage1);
	Player->SetStartLoc(pStage1->MatrixIdxToLocation(StartPoint));

	FVector2D monsterStartingLoc = pStage1->GetPortalLoc();

	// Temp. To test portal
	//Player->SetActorLocation(monsterStartingLoc + FVector2D({ 16.f, -16.f }));	// temp

	// Temp. To test collision
	/*AMushroom* monster = pLevel->SpawnActor<AMushroom>();
	monster->SetCurMap(pStage1);
	monster->SetFirstDestination({ 0, 0 });
	monster->SetActorLocation(pStage1->MatrixIdxToLocation(StartPoint) + FVector2D({ 64, 0 }));
	MonsterList.push_back(monster);*/
	
	// TODO: spawn delay
	{
		AMushroom* monster = pLevel->SpawnActor<AMushroom>();
		monster->SetCurMap(pStage1);
		monster->SetFirstDestination({ 5, 10 });
		monster->SetActorLocation(monsterStartingLoc);
		MonsterList.push_back(monster);
	}

	{
		AMushroom* monster = pLevel->SpawnActor<AMushroom>();
		monster->SetCurMap(pStage1);
		monster->SetFirstDestination({ 6, 9 });
		monster->SetActorLocation(monsterStartingLoc);
		monster->SetStartDelay(1.f);
		MonsterList.push_back(monster);
	}

	{
		ABalloon* monster = pLevel->SpawnActor<ABalloon>();
		monster->SetCurMap(pStage1);
		monster->SetFirstDestination({ 6, 9 });
		monster->SetActorLocation(monsterStartingLoc);
		monster->SetStartDelay(2.f);
		MonsterList.push_back(monster);
	}

	{
		ABalloon* monster = pLevel->SpawnActor<ABalloon>();
		monster->SetCurMap(pStage1);
		monster->SetFirstDestination({ 7, 10 });
		monster->SetActorLocation(monsterStartingLoc);
		monster->SetStartDelay(3.f);
		MonsterList.push_back(monster);
	}
	
	//pStage2->SetPortalIdx(FIntPoint(6, 10));	// temp

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();

	GameUiPtr->StartTimer();
}

void APlayGameMode::Tick(float _deltaTime)
{
	ElapsedSecs += _deltaTime;

	if (ElapsedSecs >= 1.f)
	{
		ElapsedSecs = 0.f;

		CheckTimeOver();
		CheckDeadMonster();
		if (IsAllMonstersDead())
		{
			if (CurMapPtr && !CurMapPtr->GetIsPortalOpened())
			{
				CurMapPtr->OpenPortal();
				return;
			}
			if (!isShowingResult && Player->GetIsClear())
			{
				isShowingResult = true;
				FadeOut();
			}
		}
	}

	// This requires checking the F1 key input, so the delayed time is not suitable.
	CheckGameOver();
}

void APlayGameMode::LevelChangeStart()
{
	isShowContinueScene = false;
	isShowingResult = false;
}

void APlayGameMode::CheckDeadMonster()
{
	std::list<AMonster*>::iterator it = MonsterList.begin();
	std::list<AMonster*>::iterator itEnd = MonsterList.end();
	for (; it != itEnd; ++it)
	{
		AMonster* monster = *it;
		if (monster->GetIsDestroiable())
		{
			monster->Destroy();
			it = MonsterList.erase(it);
			if (it == itEnd) break;
		}
	}
}

void APlayGameMode::GameOver()
{
	if (isShowContinueScene == false)
	{
		isShowContinueScene = true;

		StopGame();

		AFade::MainFade->SetFadeMinMax(0.f, .5f);
		AFade::MainFade->SetFadeSpeed(.5f);
		AFade::MainFade->FadeOut();

		GameOverScenePtr->ShowAndStart();
	}
	else
	{
		if (GameOverScenePtr->GetIsOver())
		{
			AFade::MainFade->SetFadeMinMax(.5f, 1.f);
			AFade::MainFade->SetFadeSpeed(.5f);
			AFade::MainFade->BindEndEvent(std::bind(&APlayGameMode::OnEndGameOverFadeOut, this));
			AFade::MainFade->FadeOut(.5f);
		}
	}
}

void APlayGameMode::CheckTimeOver()
{
	if (AGameUI::IsTimeOver())
	{
		GetWorld()->GetPawn<APlayer>()->Kill();
	}
}

void APlayGameMode::CheckGameOver()
{
	int p1Life = GameData::GetInstance().GetPlayer1Life();
	if (p1Life >= 0) return;
	if (!Player->GetIsDead()) return;

	unsigned __int8 coin = GameData::GetInstance().GetCoin();
	if (coin == 0)
	{
		GameOver();
	}
	else
	{
		if (isShowContinueScene)
		{
			if (UEngineInput::GetInst().IsDown(VK_F1))
			{
				isShowContinueScene = false;

				AFade::MainFade->SetFadeMinMax(0.f, 1.f);
				AFade::MainFade->SetFadeSpeed(1.f);
				AFade::MainFade->FadeIn(.5f);
				
				StartFromCoin();

				GameOverScenePtr->SetActive(false);
				RestartGame();
			}
		}
		else
		{
			StartFromCoin();
		}
	}
}

void APlayGameMode::StartFromCoin()
{
	GameData::GetInstance().AddCoin(-1);
	GameData::GetInstance().AddPlayer1Life(3);
	GameData::GetInstance().ResetScore();
}

void APlayGameMode::StopGame()
{
	GetWorld()->GetPawn<APlayer>()->Pause();
	GameUiPtr->StopTimer();

	std::list<AMonster*>::iterator it = MonsterList.begin();
	std::list<AMonster*>::iterator itEnd = MonsterList.end();
	for (; it != itEnd; ++it)
	{
		(*it)->Pause();
	}
}

void APlayGameMode::RestartGame()
{
	GetWorld()->GetPawn<APlayer>()->Resume();
	GameUiPtr->ResetTimer();
	GameUiPtr->StartTimer();

	std::list<AMonster*>::iterator it = MonsterList.begin();
	std::list<AMonster*>::iterator itEnd = MonsterList.end();
	for (; it != itEnd; ++it)
	{
		(*it)->Resume();
	}
}

bool APlayGameMode::IsAllMonstersDead() const
{
	return MonsterList.size() == 0;
}

void APlayGameMode::OnExplodeBomb()
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
			FIntPoint curIdx = CurMapPtr->LocationToMatrixIdx(monster->GetActorLocation());
			if (CurMapPtr->IsInSplash(curIdx))
			{
				monster->Kill();
			}
		}
	}
}

void APlayGameMode::OnEndGameOverFadeOut()
{
	UEngineAPICore::GetCore()->OpenLevel("Title");
}

// Temp
void APlayGameMode::FadeOut()
{
	AFade::MainFade->BindEndEvent(std::bind(&APlayGameMode::OnEndFadeOut, this));
	AFade::MainFade->SetFadeMinMax(0.f, .5f);
	AFade::MainFade->SetFadeSpeed(.5f);
	AFade::MainFade->FadeOut();
}

void APlayGameMode::OnEndFadeOut()
{
	ResultScene = GetWorld()->SpawnActor<AResult>();

	int lastTime = AGameUI::GetLastTime();
	ResultScene->SetLastSecs(lastTime);
	ResultScene->SetTotal(GameData::GetInstance().GetPlayer1Score());
}