#include "PreCompile.h"

#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineDebug.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/ImageManager.h>
#include <EnginePlatform/EngineSound.h>

#include "ContentsCore.h"
#include "TitleGameMode.h"
#include "TileMapGameMode.h"
#include "PlayGameMode.h"
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
	//pCore->CreateLevel<ATileMapGameMode, AActor>("TileMap");
	//pCore->OpenLevel("TileMap");
	//return;

	pCore->CreateLevel<ATitleGameMode, AActor>("Title");
	pCore->CreateLevel<APlayGameMode, APlayer>("Play");
	pCore->CreateLevel<APlayBossMode, APlayer>("Boss_Stage1");

	//pCore->OpenLevel("Title");
	pCore->OpenLevel("Play");
	//pCore->OpenLevel("Boss_Stage1");
}

void ContentsCore::Tick()
{
	// Nothing..
}

void ContentsCore::InitResources()
{
	GlobalPath path;

	/** Load images **/
	LoadImages(GlobalPath::ROOT);
	LoadImages(path.GetAppendedRootPath(GlobalPath::BACKGROUND));
	LoadImages(path.GetAppendedRootPath(GlobalPath::CHARACTER));
	LoadImages(path.GetAppendedRootPath(GlobalPath::RIDING));
	LoadImages(path.GetAppendedRootPath(GlobalPath::ENEMY));
	LoadImages(path.GetAppendedRootPath(GlobalPath::EXPLODE));
	LoadImages("Resources\\UI\\Score");	// temp
	LoadImages("Resources\\UI\\Title");	// temp
	LoadImages("Resources\\UI\\SelectCount");	// temp
	LoadImages("Resources\\Result");	// temp
	LoadImages("Resources\\CutScene");	// temp

	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::BOMB_ORG));
	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::BOMB_RED));
	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::OPENING));
	LoadImageFolders(path.GetTileStage1Path());
	LoadImageFolders(path.GetTileStage1GuidePath());
	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::ANIM_CRUMBLING_BOX));
	LoadImageFolders("Resources\\SayHi");
	LoadImageFolders("Resources\\Result\\StageClear");
	LoadImageFolders("Resources\\CutScene\\CutSceneStage1");
	LoadImageFolders("Resources\\PlayBackground");

	/* Load sounds */
	UEngineSound::InitFMOD();
	LoadSounds("Resources\\Sounds");

	/** Cutting **/
	UImageManager& imgManager = UImageManager::GetInst();
	imgManager.CuttingSprite("MainCharater_White.png", GlobalVar::BOMBERMAN_SIZE);

	imgManager.CuttingSprite("Mushroom.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("Balloon_long.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("Cookie.png", GlobalVar::BOMBERMAN_SIZE);

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
	imgManager.CuttingSprite("BlueCount.png", { 24, 48 });	// Temp
	imgManager.CuttingSprite("PinkCount.png", { 24, 48 });	// Temp
	imgManager.CuttingSprite("YellowCount.png", { 24, 48 });	// Temp

	imgManager.CuttingSprite("SelectPainterUpDown.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("SelectPainterCircle.png", GlobalVar::BOMBERMAN_SIZE);
	imgManager.CuttingSprite("brushCircle.png", { 128, 128 });

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

	std::vector<UEngineFile> files = dir.GetAllFile(false);
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string&& filePath = files[i].GetPathToString();
		UImageManager::GetInst().Load(filePath);
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