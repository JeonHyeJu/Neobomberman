#include "PreCompile.h"
#include "GlobalVar.h"
#include "BaseGameMode.h"
#include "Fade.h"
#include "Monster.h"
#include "Result.h"
#include "GameUI.h"
#include "GameData.h"
#include "GameOver.h"
#include "Player.h"
#include "BaseMap.h"

#include <EngineCore/EngineAPICore.h>
#include <EnginePlatform/EngineInput.h>
#include <EnginePlatform/EngineSound.h>

#include <EngineCore/EngineCoreDebug.h>		// for debug

ABaseGameMode::ABaseGameMode()
{

}

ABaseGameMode::~ABaseGameMode()
{

}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	ULevel* pLevel = GetWorld();

	Player = pLevel->GetPawn<APlayer>();

	GameOverScenePtr = pLevel->SpawnActor<AGameOver>();
	GameOverScenePtr->SetActive(false);
}

void ABaseGameMode::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	CheckCheat();

	ElapsedSecs += _deltaTime;
	if (ElapsedSecs >= .5f)
	{
		ElapsedSecs = 0.f;

		CheckTimeOver();
		CheckDeadMonster();
		if (IsAllMonstersDead())
		{
			FinishGame();
		}
	}

	// This requires checking the F1 key input, so the delayed time is not suitable.
	CheckGameOver();

	DebugExlosionEdge();
}

void ABaseGameMode::LevelChangeStart()
{
	Super::LevelChangeStart();

	IsShowContinueScene = false;
	IsShowingResult = false;
}

void ABaseGameMode::InitResultScene(std::string_view _nextLevel, std::string_view _rImage)
{
	NextLevelName = _nextLevel;
	RImageName = _rImage;
}

bool ABaseGameMode::IsAllMonstersDead() const
{
	return MonsterList.size() == 0;
}

void ABaseGameMode::CheckTimeOver()
{
	if (AGameUI::IsTimeOver())
	{
		Player->Kill();
	}
}

void ABaseGameMode::CheckDeadMonster()
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

void ABaseGameMode::CheckGameOver()
{
	int p1Life = GameData::GetInstance().GetPlayer1Life();
	if (p1Life >= 0) return;
	if (!Player->GetIsDead()) return;

	unsigned __int8 coin = GameData::GetInstance().GetCoin();
	if (coin == 0)
	{
		if (UEngineInput::GetInst().IsDown(VK_RETURN))
		{
			UEngineSound::Play(SFXCoin, -1, 0, false);
			GameData::GetInstance().AddCoin(1);
			return;
		}

		GameOver();
	}
	else
	{
		if (IsShowContinueScene)
		{
			if (UEngineInput::GetInst().IsDown(VK_F1) || UEngineInput::GetInst().IsDown(VK_RETURN))
			{
				IsShowContinueScene = false;

				AFade::MainFade->SetFadeMinMax(0.f, 1.f);
				AFade::MainFade->SetFadeSpeed(1.f);
				AFade::MainFade->FadeIn(.5f);

				RevivalFromCoin();

				GameOverScenePtr->SetActive(false);
				GameOverScenePtr->Reset();
				RestartGame();
			}
		}
		else
		{
			RevivalFromCoin();
		}
	}
}

void ABaseGameMode::RevivalFromCoin()
{
	GameData& gameData = GameData::GetInstance();
	gameData.AddCoin(-1);
	gameData.AddPlayer1Life(3);
	gameData.ResetScore();
}

void ABaseGameMode::GameOver()
{
	if (IsShowContinueScene == false)
	{
		IsShowContinueScene = true;

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
			AFade::MainFade->BindEndEvent(std::bind(&ABaseGameMode::_GoToTitle, this));
			AFade::MainFade->FadeOut(.5f);
		}
	}
}

void ABaseGameMode::StopGame()
{
	Player->Pause();
	AGameUI::StopTimer();

	std::list<AMonster*>::iterator it = MonsterList.begin();
	std::list<AMonster*>::iterator itEnd = MonsterList.end();
	for (; it != itEnd; ++it)
	{
		(*it)->Pause();
	}
}

void ABaseGameMode::RestartGame()
{
	Player->Resume();
	AGameUI::ResetTimer();
	AGameUI::StartTimer();

	std::list<AMonster*>::iterator it = MonsterList.begin();
	std::list<AMonster*>::iterator itEnd = MonsterList.end();
	for (; it != itEnd; ++it)
	{
		(*it)->Resume();
	}
}

void ABaseGameMode::ShowResult()
{
	AFade::MainFade->BindEndEvent(std::bind(&ABaseGameMode::_ShowResultScene, this));
	AFade::MainFade->SetFadeMinMax(0.f, .5f);
	AFade::MainFade->SetFadeSpeed(.5f);
	AFade::MainFade->FadeOut();
}

void ABaseGameMode::_ShowResultScene()
{
	AResult* resultScene = GetWorld()->SpawnActor<AResult>();
	resultScene->SetNextLevel(NextLevelName);
	resultScene->ChangeRImage(RImageName, 0);

	resultScene->SetLastSecs(AGameUI::GetLastTime());
	resultScene->SetTotal(GameData::GetInstance().GetPlayer1Score());
}

void ABaseGameMode::_GoToTitle()
{
	UEngineSound::AllSoundStop();
	UEngineAPICore::GetCore()->OpenLevel("Title");
}

void ABaseGameMode::OnExplodeBomb()
{
	if (MapPtr == nullptr) return;

	SplashTileIdxsBackup = MapPtr->GetSplashTileIdxs();
	IsSplashCheck = true;

	CheckAfterExplosion(.4f);
}

bool ABaseGameMode::IsInExplosion4Edges(const FVector2D& _loc, const FVector2D& _bodySize, const URect& _margin)
{
	FVector2D nextPosLT = _loc + FVector2D{ _margin.Left, _margin.Top };
	FVector2D nextPosRT = _loc + FVector2D{ _bodySize.X - _margin.Right, _margin.Top };;
	FVector2D nextPosLB = _loc + FVector2D{ _margin.Left, _bodySize.Y - _margin.Bottom };
	FVector2D nextPosRB = _loc + FVector2D{ _bodySize.X - _margin.Right, _bodySize.Y - _margin.Bottom };

	FIntPoint nextIdxLT = MapPtr->LocationToMatrixIdx(nextPosLT);
	FIntPoint nextIdxRT = MapPtr->LocationToMatrixIdx(nextPosRT);
	FIntPoint nextIdxLB = MapPtr->LocationToMatrixIdx(nextPosLB);
	FIntPoint nextIdxRB = MapPtr->LocationToMatrixIdx(nextPosRB);

	int subX = nextIdxRB.X - nextIdxLT.X;
	int subY = nextIdxRB.Y - nextIdxLT.Y;
	for (int x = 0; x <= subX; ++x)
	{
		for (int y = 0; y <= subY; ++y)
		{
			FIntPoint point = { nextIdxLT.X + x, nextIdxLT.Y + y };

			bool isInSplash = MapPtr->IsInSplashWithVector(point, SplashTileIdxsBackup);
			//OutputDebugString(("interIdxs[" + std::to_string(x) + ", " + std::to_string(y) + "] : " + std::to_string(point.X) + ", " + std::to_string(point.Y) + "..." + (isInSplash ? "isInSplash O" : "isInSplash X") + "\n").c_str());
			if (isInSplash)
			{
				return true;
			}
		}
	}

	return false;
}

// Hack code.. lasting explosion impact..
void ABaseGameMode::CheckAfterExplosion(float _deltaTime)
{
	if (MapPtr == nullptr) return;
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
				const FVector2D& monsterLoc = monster->GetActorLocation();
				const FVector2D& damageSize = monster->GetDamageSize();
				const URect& damageMargin = monster->GetDamageMargin();

				if (IsInExplosion4Edges(monsterLoc, damageSize, damageMargin))
				{
					monster->Damaged();
				}
			}
		}

		// Check player
		{
			const FVector2D& playerLoc = Player->GetActorLocation();
			const FVector2D& damageSize = Player->GetDamageSize();
			const URect& damageMargin = Player->GetDamageMargin();

			if (IsInExplosion4Edges(playerLoc, damageSize, damageMargin))
			{
				Player->Kill();
			}
		}
		
		// Check item
		for (size_t i = 0, size = SplashTileIdxsBackup.size(); i < size; ++i)
		{
			FIntPoint pt = SplashTileIdxsBackup[i];
			if (MapPtr->HasShowingItem(pt))
			{
				EItem item = MapPtr->PopItem(pt);	// Remove
			}
		}

		executeCnt++;
	}
}

/* Cheat */
void ABaseGameMode::DebugExlosionEdge()
{
	std::list<AMonster*>::iterator it = MonsterList.begin();
	std::list<AMonster*>::iterator itEnd = MonsterList.end();
	for (; it != itEnd; ++it)
	{
		AMonster* monster = *it;

		const FVector2D& monsterLoc = monster->GetActorLocation();
		const FVector2D& damageSize = monster->GetDamageSize();
		const URect& damageMargin = monster->GetDamageMargin();

		FVector2D nextPosLT = monsterLoc + FVector2D{ damageMargin.Left, damageMargin.Top };
		FVector2D nextPosRT = monsterLoc + FVector2D{ damageSize.X - damageMargin.Right, damageMargin.Top };;
		FVector2D nextPosLB = monsterLoc + FVector2D{ damageMargin.Left, damageSize.Y - damageMargin.Bottom };
		FVector2D nextPosRB = monsterLoc + FVector2D{ damageSize.X - damageMargin.Right, damageSize.Y - damageMargin.Bottom };

		FTransform TransLT, TransRT, TransLB, TransRB;

		TransLT.Location = nextPosLT;
		TransRT.Location = nextPosRT;
		TransLB.Location = nextPosLB;
		TransRB.Location = nextPosRB;

		TransLT.Scale = { 6, 6 };
		TransRT.Scale = { 6, 6 };
		TransLB.Scale = { 6, 6 };
		TransRB.Scale = { 6, 6 };

		UEngineDebug::CoreDebugRender(TransLT, UEngineDebug::EDebugPosType::Circle);
		UEngineDebug::CoreDebugRender(TransRT, UEngineDebug::EDebugPosType::Circle);
		UEngineDebug::CoreDebugRender(TransLB, UEngineDebug::EDebugPosType::Circle);
		UEngineDebug::CoreDebugRender(TransRB, UEngineDebug::EDebugPosType::Circle);
	}

	{
		const FVector2D& playerLoc = Player->GetActorLocation();
		const FVector2D& damageSize = Player->GetDamageSize();
		const URect& damageMargin = Player->GetDamageMargin();

		FVector2D nextPosLT = playerLoc + FVector2D{ damageMargin.Left, damageMargin.Top };
		FVector2D nextPosRT = playerLoc + FVector2D{ damageSize.X - damageMargin.Right, damageMargin.Top };;
		FVector2D nextPosLB = playerLoc + FVector2D{ damageMargin.Left, damageSize.Y - damageMargin.Bottom };
		FVector2D nextPosRB = playerLoc + FVector2D{ damageSize.X - damageMargin.Right, damageSize.Y - damageMargin.Bottom };

		FTransform TransLT, TransRT, TransLB, TransRB;

		TransLT.Location = nextPosLT;
		TransRT.Location = nextPosRT;
		TransLB.Location = nextPosLB;
		TransRB.Location = nextPosRB;

		TransLT.Scale = { 6, 6 };
		TransRT.Scale = { 6, 6 };
		TransLB.Scale = { 6, 6 };
		TransRB.Scale = { 6, 6 };

		UEngineDebug::CoreDebugRender(TransLT, UEngineDebug::EDebugPosType::Circle);
		UEngineDebug::CoreDebugRender(TransRT, UEngineDebug::EDebugPosType::Circle);
		UEngineDebug::CoreDebugRender(TransLB, UEngineDebug::EDebugPosType::Circle);
		UEngineDebug::CoreDebugRender(TransRB, UEngineDebug::EDebugPosType::Circle);
	}
}

void ABaseGameMode::CheckCheat()
{
	if (UEngineInput::GetInst().IsDown('N'))
	{
		if (!IsShowingResult && !IsAllMonstersDead())
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
			MapPtr->CheatDestoyAllBoxes();
		}
	}
}