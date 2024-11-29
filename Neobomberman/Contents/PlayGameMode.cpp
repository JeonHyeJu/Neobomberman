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
#include "HurryUp.h"
#include "StageTitle.h"
#include "Fade.h"

#include <EngineCore/Level.h>
#include <EngineCore/EngineAPICore.h>
#include <EnginePlatform/EngineInput.h>
#include <EnginePlatform/EngineSound.h>

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
	std::vector<EItem> itemList = { EItem::BOMB, EItem::BOMB, EItem::SPEED };

	APlayMap* pStage1 = pLevel->SpawnActor<APlayMap>();
	pStage1->SetPortalIdx(PORTAL_IDX_STAGE_1);
	pStage1->BindExplodeEvent(std::bind(&APlayGameMode::OnExplodeBomb, this));
	CurMapPtr = pStage1;

	pStage1->InitMap();
	pStage1->SetItems(itemList);	// Must set after InitMap

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

	AStageTitle* stageTitle = pLevel->SpawnActor<AStageTitle>();
	stageTitle->SetSubStage(1);

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();

	GameUiPtr->StartTimer();
	UEngineSound::Play(SFXBg);
}

void APlayGameMode::CheckAndPlayBgSound()
{
	if (isShowContinueScene) return;
	if (isShowingResult) return;

	if (GameUiPtr->GetIsHalfTime())
	{
		// Temp
		static bool onlyOnce = true;
		if (onlyOnce)
		{
			onlyOnce = false;
			AHurryUp* hurryUp = GetWorld()->SpawnActor<AHurryUp>();
			UEngineSound::StopPlayer(SFXBg);
			UEngineSound::Play(SFXAlertHurryUp);
			UEngineSound::Play(SFXBgHurryUp, -1, 100);
		}
	}
	else
	{
		if (!UEngineSound::IsPlaying(SFXBg))
		{
			UEngineSound::Play(SFXBg, 7700);
		}
	}
}

void APlayGameMode::Tick(float _deltaTime)
{
	ElapsedSecs += _deltaTime;

	CheckCheat();
	CheckAfterExplosion(_deltaTime);
	CheckAndPlayBgSound();

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
				UEngineSound::Play(SFXOpenPortal);
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

void APlayGameMode::GameOver()
{
	if (isShowContinueScene == false)
	{
		isShowContinueScene = true;

		StopGame();

		AFade::MainFade->SetFadeMinMax(0.f, .5f);
		AFade::MainFade->SetFadeSpeed(.5f);
		AFade::MainFade->FadeOut();

		UEngineSound::AllSoundStop();
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

void APlayGameMode::CheckTimeOver()
{
	if (GameUiPtr->IsTimeOver())
	{
		Player->Kill();
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
		if (UEngineInput::GetInst().IsDown(VK_RETURN))
		{
			UEngineSound::Play("Coin.mp3", -1, 0, false);
			GameData::GetInstance().AddCoin(1);
			return;
		}

		GameOver();
	}
	else
	{
		if (isShowContinueScene)
		{
			if (UEngineInput::GetInst().IsDown(VK_F1) || UEngineInput::GetInst().IsDown(VK_RETURN))
			{
				isShowContinueScene = false;

				AFade::MainFade->SetFadeMinMax(0.f, 1.f);
				AFade::MainFade->SetFadeSpeed(1.f);
				AFade::MainFade->FadeIn(.5f);
				
				StartFromCoin();

				GameOverScenePtr->SetActive(false);
				GameOverScenePtr->Reset();
				RestartGame();
			}
		}
		else
		{
			StartFromCoin();
		}
	}
}

void APlayGameMode::CheckCheat()
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
		else
		{
			CurMapPtr->CheatDestoyAllBoxes();
		}
	}
}

void APlayGameMode::StartFromCoin()
{
	GameData& gameData = GameData::GetInstance();
	gameData.AddCoin(-1);
	gameData.AddPlayer1Life(3);
	gameData.ResetScore();
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

// Hack code.. lasting explosion impact..
void APlayGameMode::CheckAfterExplosion(float _deltaTime)
{
	if (CurMapPtr == nullptr) return;
	if (!IsSplashCheck) return;
	if (SplashTileIdxsBackup.size() == 0) return;

	static float elapsedSecs = 0.f;
	static int executeCnt = 0;

	elapsedSecs += _deltaTime;

	if (elapsedSecs >= .4f)	// match with explosion anim
	{
		elapsedSecs = 0.f;
		if (executeCnt > 3)
		{
			executeCnt = 0;
			IsSplashCheck = false;
			return;
		}

		// Check monsters
		std::list<AMonster*>::iterator it = MonsterList.begin();
		std::list<AMonster*>::iterator itEnd = MonsterList.end();
		for (; it != itEnd; ++it)
		{
			AMonster* monster = *it;
			if (monster->GetCanHit())
			{
				FVector2D monsterLoc = monster->GetActorLocation();
				FVector2D margin{ 10, 10 };

				FVector2D nextPosLT = monsterLoc + margin;
				FVector2D nextPosRT = FVector2D{ nextPosLT.X + GlobalVar::BOMB_SIZE.hX() * .5f, nextPosLT.Y };
				FVector2D nextPosLB = FVector2D{ nextPosLT.X, monsterLoc.Y + GlobalVar::BOMB_SIZE.hY() * .4f + margin.Y };
				FVector2D nextPosRB = FVector2D{ nextPosRT.X, nextPosLB.Y };

				FIntPoint nextIdxLT = CurMapPtr->LocationToMatrixIdx(nextPosLT);
				FIntPoint nextIdxRT = CurMapPtr->LocationToMatrixIdx(nextPosRT);
				FIntPoint nextIdxLB = CurMapPtr->LocationToMatrixIdx(nextPosLB);
				FIntPoint nextIdxRB = CurMapPtr->LocationToMatrixIdx(nextPosRB);

				bool isInSplashLT = CurMapPtr->IsInSplashWithVector(nextIdxLT, SplashTileIdxsBackup);
				bool isInSplashRT = CurMapPtr->IsInSplashWithVector(nextIdxRT, SplashTileIdxsBackup);
				bool isInSplashLB = CurMapPtr->IsInSplashWithVector(nextIdxLB, SplashTileIdxsBackup);
				bool isInSplashRB = CurMapPtr->IsInSplashWithVector(nextIdxRB, SplashTileIdxsBackup);

				/*FIntPoint curIdx = CurMapPtr->LocationToMatrixIdx(monster->GetActorLocation());
				if (CurMapPtr->IsInSplashWithVector(curIdx, SplashTileIdxsBackup))*/

				if (isInSplashLT || isInSplashRT || isInSplashLB || isInSplashRB)
				{
					monster->Kill();
				}
			}
		}

		// Check player
		FVector2D playerLoc = Player->GetActorLocation();
		FVector2D margin{ 10, 10 };

		FVector2D nextPosLT = playerLoc + margin;
		FVector2D nextPosRT = FVector2D{ nextPosLT.X + GlobalVar::BOMBERMAN_SIZE.hX() * .5f, nextPosLT.Y};
		FVector2D nextPosLB = FVector2D{ nextPosLT.X, playerLoc.Y + GlobalVar::BOMBERMAN_SIZE.hY() *.4f + margin.Y };
		FVector2D nextPosRB = FVector2D{ nextPosRT.X, nextPosLB.Y };

		FIntPoint nextIdxLT = CurMapPtr->LocationToMatrixIdx(nextPosLT);
		FIntPoint nextIdxRT = CurMapPtr->LocationToMatrixIdx(nextPosRT);
		FIntPoint nextIdxLB = CurMapPtr->LocationToMatrixIdx(nextPosLB);
		FIntPoint nextIdxRB = CurMapPtr->LocationToMatrixIdx(nextPosRB);

		bool isInSplashLT = CurMapPtr->IsInSplashWithVector(nextIdxLT, SplashTileIdxsBackup);
		bool isInSplashRT = CurMapPtr->IsInSplashWithVector(nextIdxRT, SplashTileIdxsBackup);
		bool isInSplashLB = CurMapPtr->IsInSplashWithVector(nextIdxLB, SplashTileIdxsBackup);
		bool isInSplashRB = CurMapPtr->IsInSplashWithVector(nextIdxRB, SplashTileIdxsBackup);

		/*OutputDebugString(("nextIdxLT : " + std::to_string(nextIdxLT.X) + ", " + std::to_string(nextIdxLT.Y) + "\n").c_str());
		OutputDebugString(("nextIdxRT : " + std::to_string(nextIdxRT.X) + ", " + std::to_string(nextIdxRT.Y) + "\n").c_str());
		OutputDebugString(("nextIdxLB : " + std::to_string(nextIdxLB.X) + ", " + std::to_string(nextIdxLB.Y) + "\n").c_str());
		OutputDebugString(("nextIdxRB : " + std::to_string(nextIdxRB.X) + ", " + std::to_string(nextIdxRB.Y) + "\n").c_str());
		OutputDebugString("----------------------------------\n");*/
		if (isInSplashLT || isInSplashRT || isInSplashLB || isInSplashRB)
		{
			Player->Kill();
		}

		// Check item
		for (size_t i = 0, size = SplashTileIdxsBackup.size(); i < size; ++i)
		{
			FIntPoint pt = SplashTileIdxsBackup[i];
			if (CurMapPtr->HasShowingItem(pt))
			{
				EItem item = CurMapPtr->PopItem(pt);	// Remove
			}
		}

		executeCnt++;
	}
}

void APlayGameMode::OnExplodeBomb()
{
	if (CurMapPtr == nullptr) return;

	SplashTileIdxsBackup = CurMapPtr->GetSplashTileIdxs();
	IsSplashCheck = true;

	CheckAfterExplosion(.4f);
}

void APlayGameMode::OnEndGameOverFadeOut()
{
	UEngineSound::AllSoundStop();
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
	ResultScene->SetNextLevel("Boss_Stage1");

	int lastTime = AGameUI::GetLastTime();
	ResultScene->SetLastSecs(lastTime);
	ResultScene->SetTotal(GameData::GetInstance().GetPlayer1Score());
}