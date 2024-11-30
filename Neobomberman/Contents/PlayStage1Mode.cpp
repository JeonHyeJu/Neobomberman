#include "PreCompile.h"
#include "PlayStage1Mode.h"
#include "GameData.h"
#include "GameUI.h"
#include "Stage1Map.h"
#include "Player.h"
#include "Monster.h"
#include "Mushroom.h"
#include "Balloon.h"
#include "HurryUp.h"
#include "StageTitle.h"
#include "Fade.h"

#include <EngineCore/Level.h>
#include <EnginePlatform/EngineSound.h>

APlayStage1Mode::APlayStage1Mode()
{
	InitResultScene("Boss_Stage1", "ResultImage1.png");
}

APlayStage1Mode::~APlayStage1Mode()
{
}

void APlayStage1Mode::BeginPlay()
{
	ABaseGameMode::BeginPlay();

	ULevel* pLevel = GetWorld();

	GameUiPtr = pLevel->SpawnActor<AGameUI>();

	Player = pLevel->GetPawn<APlayer>();
	Player->SetGameUI(GameUiPtr);

	/* Stage 1-1 */
	std::vector<EItem> itemList = { EItem::BOMB, EItem::BOMB, EItem::SPEED };

	AStage1Map* pStage1 = pLevel->SpawnActor<AStage1Map>();
	pStage1->Initialize(itemList, PORTAL_IDX_STAGE_1);
	pStage1->BindExplodeEvent(std::bind(&APlayStage1Mode::OnExplodeBomb, this));
	MapPtr = pStage1;

	Player->SetCurMap(pStage1);
	Player->SetStartLoc(pStage1->MatrixIdxToLocation(StartPoint));

	// Temp. To test portal
	//Player->SetActorLocation(pStage1->GetPortalLoc() + FVector2D({ 16.f, -16.f }));

	// Temp. To test collision
	//SpawnMonster<AMushroom>(pStage1, pStage1->MatrixIdxToLocation(StartPoint) + FVector2D({ 64, 0 }), 0.f, { 2, 0 });

	FVector2D monsterStartingLoc = pStage1->GetPortalLoc();
	SpawnMonster<AMushroom>(pStage1, monsterStartingLoc, 0.f, { 5, 10 });
	SpawnMonster<AMushroom>(pStage1, monsterStartingLoc, .5f, { 6, 9 });
	SpawnMonster<ABalloon>(pStage1, monsterStartingLoc, 1.f, { 6, 9 });
	SpawnMonster<ABalloon>(pStage1, monsterStartingLoc, 1.5f, { 7, 10 });

	AStageTitle* stageTitle = pLevel->SpawnActor<AStageTitle>();
	stageTitle->SetSubStage(1);

	AFade* fade = pLevel->SpawnActor<AFade>();
	fade->FadeIn();

	GameUiPtr->StartTimer();
	UEngineSound::Play(SFXBg);
}

void APlayStage1Mode::Tick(float _deltaTime)
{
	ABaseGameMode::Tick(_deltaTime);

	ElapsedSecs += _deltaTime;

	CheckAfterExplosion(_deltaTime);
	CheckAndPlayBgSound();
}

void APlayStage1Mode::FinishGame()
{
	if (MapPtr && !MapPtr->GetIsPortalOpened())
	{
		MapPtr->OpenPortal();
		UEngineSound::Play(SFXOpenPortal);
		return;
	}
	if (!IsShowingResult && Player->GetIsClear())
	{
		IsShowingResult = true;
		ShowResult();
	}
}

void APlayStage1Mode::CheckAndPlayBgSound()
{
	if (IsShowContinueScene) return;
	if (IsShowingResult) return;

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