#include "PreCompile.h"
#include "PlayBossMode.h"
#include "GameUI.h"
#include "BossMap.h"
#include "HoopGhost.h"
#include "Monster.h"
#include "Player.h"
#include "Result.h"
#include "GameData.h"
#include "Fade.h"

#include <EngineCore/Level.h>

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
	//Player->SetCollisionImage("Bg_1-Col.png");

	/* Stage 1-1 */
	ABossMap* pBossMap = pLevel->SpawnActor<ABossMap>();
	pBossMap->InitMap();

	Player->SetCurMap(pBossMap);
	FVector2D playerStartLoc = pBossMap->MatrixIdxToLocation(StartPoint);	// Temp
	Player->SetActorLocation(playerStartLoc + GlobalVar::BOMBERMAN_SIZE.Half().Half());

	AHoopGhost* boss = pLevel->SpawnActor<AHoopGhost>();
	boss->SetCurMap(pBossMap);
	boss->SetFirstDestination({ 0, 0 });
	boss->SetActorLocation({ 256, 156 });	// Temp
	MonsterList.push_back(boss);

	AFade* fade = GetWorld()->SpawnActor<AFade>();
	fade->FadeIn();

	gameUI->StartTimer();
}

void APlayBossMode::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	static float elpasedSecs = 0.f;	// Temp
	elpasedSecs += _deltaTime;

	if (elpasedSecs > .5f)
	{
		elpasedSecs = 0.f;

		if (!isShowingResult && Player->GetIsClear())
		{
			isShowingResult = true;
			FadeOut();
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

	int lastTime = AGameUI::GetLastTime();
	ResultScene->SetLastSecs(lastTime);
	ResultScene->SetTotal(GameData::GetInstance().GetPlayer1Score());
}