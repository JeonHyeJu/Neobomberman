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
	pCore->GetMainWindow().SetWindowPosAndScale({ 0, 0 }, FVector2D(GlobalVar::WINDOW_SIZE));	// You must call this.

	InitResources();

	/** For creating tilemap. **/
	//pCore->CreateLevel<ATileMapGameMode, AActor>("TileMap");
	//pCore->OpenLevel("TileMap");
	//return;

	pCore->CreateLevel<APlayGameMode, APlayer>("Play");
	pCore->CreateLevel<ATitleGameMode, AActor>("Title");

	pCore->OpenLevel("Title");
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

	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::BOMB_ORG));
	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::BOMB_RED));
	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::OPENING));
	LoadImageFolders(path.GetTileStage1Path());
	LoadImageFolders(path.GetTileStage1GuidePath());
	LoadImageFolders(path.GetAppendedRootPath(GlobalPath::ANIM_CRUMBLING_BOX));

	/* Load sounds */
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