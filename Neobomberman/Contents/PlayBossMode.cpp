#include "PreCompile.h"
#include "PlayBossMode.h"
#include "BossMap.h"
#include "HoopGhost.h"
#include "Player.h"
#include "StageTitle.h"
#include "Fade.h"

#include <EngineCore/Level.h>
#include <EnginePlatform/EngineInput.h>
#include <EnginePlatform/EngineSound.h>

APlayBossMode::APlayBossMode()
{
	InitResultScene("Ending", "ResultImage2.png");
	InitBgMusic("Stage1Boss.mp3");
}

APlayBossMode::~APlayBossMode()
{
}

void APlayBossMode::BeginPlay()
{
	ABaseGameMode::BeginPlay();

	ULevel* pLevel = GetWorld();

	Player = pLevel->GetPawn<APlayer>();

	/* Stage 1-1 */
	ABossMap* pBossMap = pLevel->SpawnActor<ABossMap>();
	pBossMap->Initialize();
	pBossMap->BindExplodeEvent(std::bind(&APlayBossMode::OnExplodeBomb, this));
	MapPtr = pBossMap;

	FIntPoint PlayerStartPoint{ 6, 8 };
	Player->SetCurMap(pBossMap);
	Player->SetStartLoc(pBossMap->MatrixIdxToLocation(PlayerStartPoint));

	SpawnMonster<AHoopGhost>(pBossMap, { 256, 126 }, 3.f);

	AStageTitle* stageTitle = pLevel->SpawnActor<AStageTitle>();
	stageTitle->SetSubStage(2);

	AFade* fade = GetWorld()->SpawnActor<AFade>();
	fade->FadeIn();
}

void APlayBossMode::Tick(float _deltaTime)
{
	ABaseGameMode::Tick(_deltaTime);
}

void APlayBossMode::OnFinishGame()
{
	Player->ShowWinnerPose();
}

void APlayBossMode::FinishingGame(float _deltaTime)
{
	ShowResult();
}