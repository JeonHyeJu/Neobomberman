#include "PreCompile.h"

#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineDebug.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/ImageManager.h>
#include <EnginePlatform/EngineSound.h>

#include "ContentsCore.h"
#include "TitleGameMode.h"
#include "EndingGameMode.h"
#include "TileMapGameMode.h"
#include "BattleSelectMode.h"
#include "BattlePlayMode.h"
#include "PlayStage1Mode.h"
#include "PlayBossMode.h"
#include "Player.h"
#include "GlobalVar.h"

ContentsCore::ContentsCore()
{
}

ContentsCore::~ContentsCore()
{
}

void ContentsCore::BeginPlay()
{
	UEngineAPICore* pCore = UEngineAPICore::GetCore();

	pCore->GetMainWindow().SetWindowTitle("Neobomberman");
	pCore->GetMainWindow().SetWindowPosAndScale(GlobalVar::WINDOW_SIZE.Half(), FVector2D(GlobalVar::WINDOW_SIZE));	// You must call this.

	InitResources();

	/** For creating tilemap. **/
	/*pCore->CreateLevel<ATileMapGameMode, AActor>("TileMap");
	pCore->OpenLevel("TileMap");
	return;*/

	pCore->CreateLevel<ATitleGameMode, AActor>("Title");
	pCore->CreateLevel<AEndingGameMode, AActor>("Ending");
	pCore->CreateLevel<APlayStage1Mode, APlayer>("Play");
	pCore->CreateLevel<APlayBossMode, APlayer>("Boss_Stage1");
	pCore->CreateLevel<ABattleSelectMode, AActor>("BattleSelect");
	pCore->CreateLevel<ABattlePlayMode, APlayer>("BattlePlay");

	pCore->OpenLevel("Title");
	//pCore->OpenLevel("Play");
	//pCore->OpenLevel("Boss_Stage1");
	//pCore->OpenLevel("Ending");
	//pCore->OpenLevel("BattleSelect");
	//pCore->OpenLevel("BattlePlay");
}

void ContentsCore::Tick()
{
	UEngineSound::Update();
}

void ContentsCore::InitResources()
{
	GlobalPath path;

	/** Load images **/
	LoadImages(GlobalPath::ROOT);
	LoadImages(path.GetAppendedRootPath("Background"));
	LoadImages(path.GetAppendedRootPath("Character_32x64"));
	LoadImages(path.GetAppendedRootPath("Riding_32x32"));
	LoadImages(path.GetAppendedRootPath("Enemies_32x32"));
	LoadImages(path.GetAppendedRootPath("Explode"));
	LoadImages(path.GetAppendedRootPath("Result"));
	LoadImages(path.GetAppendedRootPath("CutScene"));
	LoadImages(path.GetAppendedRootPath("Closing"));
	LoadImages(path.GetAppendedRootPath("Items"));
	LoadImages(path.GetAppendedRootPath("UI\\Score"));
	LoadImages(path.GetAppendedRootPath("UI\\Title"));
	LoadImages(path.GetAppendedRootPath("UI\\SelectCount"));

	LoadImageFolders(path.GetAppendedRootPath("BombOrg"));
	LoadImageFolders(path.GetAppendedRootPath("BombRed"));
	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::OPENING));
	LoadImageFolders(path.GetTileStage1Path());
	LoadImageFolders(path.GetTileStage1GuidePath());
	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::ANIM_CRUMBLING_BOX));
	LoadImageFolders(path.GetAppendedRootPath("SayHi"));
	LoadImageFolders(path.GetAppendedRootPath("Result\\StageClear"));
	LoadImageFolders(path.GetAppendedRootPath("CutScene\\CutSceneStage1"));
	LoadImageFolders(path.GetAppendedRootPath("PlayBackground"));
	LoadImageFolders(path.GetAppendedRootPath("HoopGhostCloud"));

	/* Load sounds */
	LoadSounds(path.GetAppendedRootPath("Sounds"));

	/** Cutting **/
	UImageManager& imgManager = UImageManager::GetInst();
	imgManager.CuttingSprite("MainCharater_White.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("MainCharater_Black.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("MainCharater_Blue.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("MainCharater_Red.png", GlobalVar::BOMBERMAN_SIZE);

	imgManager.CuttingSprite("Mushroom.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("Balloon.png", GlobalVar::BOMBERMAN_SIZE);

	imgManager.CuttingSprite("ExplodeCenter.png", GlobalVar::BOMB_SIZE);

	imgManager.CuttingSprite("ExplodeUp.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("ExplodeDown.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("ExplodeLeft.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("ExplodeRight.png", GlobalVar::BOMB_SIZE);

	imgManager.CuttingSprite("ExplodeUpMid.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("ExplodeDownMid.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("ExplodeLeftMid.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("ExplodeRightMid.png", GlobalVar::BOMB_SIZE);

	imgManager.CuttingSprite("TimeCount.png", GlobalVar::TIME_COUNT_SIZE);
	imgManager.CuttingSprite("Title_countdown_24x24.png", GlobalVar::TITLE_TIME_COUNT_SIZE);
	imgManager.CuttingSprite("BlueCount.png", GlobalVar::SELECT_TIME_COUNT_SIZE);
	imgManager.CuttingSprite("PinkCount.png", GlobalVar::SELECT_TIME_COUNT_SIZE);
	imgManager.CuttingSprite("YellowCount.png", GlobalVar::SELECT_TIME_COUNT_SIZE);

	imgManager.CuttingSprite("SelectPainterUpDown.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("SelectPainterCircle.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("brushCircle.png", GlobalVar::BRUSH_CIRCLE);

	imgManager.CuttingSprite("ClosedPortal.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("OpenedPortal.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("OpenedPortalMove.png", GlobalVar::BOMB_SIZE);

	imgManager.CuttingSprite("ResultCount.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("BarScoreNumber.png", GlobalVar::BAR_SCORE_NUMBER);
	imgManager.CuttingSprite("MonsterCloud.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("MonsterScore.png", GlobalVar::BOMB_SIZE);

	imgManager.CuttingSprite("Continue.png", { 534, 64 });
	imgManager.CuttingSprite("ContinueNumber.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("GameOver.png", GlobalVar::BOMBERMAN_SIZE);

	imgManager.CuttingSprite("LeftBears.png", { 96, 196 });
	imgManager.CuttingSprite("RightBears.png", { 96, 196 });
	imgManager.CuttingSprite("HoopGhost.png", GlobalVar::HOOPGHOST_SIZE);
	imgManager.CuttingSprite("HurryUp.png", { 510, 64 });
	imgManager.CuttingSprite("Items.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("BgBottom.png", { 416, 32 });
	imgManager.CuttingSprite("Siren.png", GlobalVar::BOMB_SIZE);
	imgManager.CuttingSprite("Explanation.png", { 360, 360 });
	imgManager.CuttingSprite("CharacterNames.png", { 128, 16 });
	imgManager.CuttingSprite("BalloonBomberman.png", { 156, 156 });
	imgManager.CuttingSprite("StageTitle.png", { 150, 64 });
	imgManager.CuttingSprite("SelectWhite.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("SelectBlack.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("SelectRed.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("SelectBlue.png", GlobalVar::BOMBERMAN_SIZE);

	imgManager.CuttingSprite("CrumblingWoodBox.png", GlobalVar::BOMB_SIZE);
}

void ContentsCore::LoadImages(std::string_view _path)
{
	UEngineDirectory dir;
	bool isFind = dir.MoveRelative(_path);

	if (!isFind)
	{
		MSGASSERT("이미지 리소스 폴더를 찾지 못했습니다.");
		return;
	}

	UImageManager& imgManager = UImageManager::GetInst();

	std::vector<UEngineFile> files = dir.GetAllFile(false);
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string&& filePath = files[i].GetPathToString();
		imgManager.Load(filePath);
	}
}

void ContentsCore::LoadImageFolders(std::string_view _path, std::string_view _append)
{
	UEngineDirectory dir;
	bool isFind = dir.MoveRelative(_path);

	if (!isFind)
	{
		MSGASSERT("이미지 리소스 폴더를 찾지 못했습니다.");
		return;
	}

	std::string&& folderPath = dir.GetPathToString();
	UImageManager::GetInst().LoadFolder(folderPath);
}

void ContentsCore::LoadSounds(std::string_view _path)
{
	UEngineDirectory dir;
	bool isFind = dir.MoveRelative(_path);

	if (!isFind)
	{
		MSGASSERT("사운드 리소스 폴더를 찾지 못했습니다.");
		return;
	}

	std::vector<UEngineFile> files = dir.GetAllFile();
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string FilePath = files[i].GetPathToString();
		UEngineSound::Load(FilePath);
	}
}