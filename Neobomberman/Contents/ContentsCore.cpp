#include "PreCompile.h"
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineDebug.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/ImageManager.h>
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

	/** Load resources **/
	LoadResource(GlobalPath::ROOT);
	LoadResource(path.GetAppendedRootPath(GlobalPath::BACKGROUND));
	LoadResource(path.GetAppendedRootPath(GlobalPath::CHARACTER));
	LoadResource(path.GetAppendedRootPath(GlobalPath::RIDING));
	LoadResource(path.GetAppendedRootPath(GlobalPath::ENEMY));
	LoadResource(path.GetAppendedRootPath(GlobalPath::EXPLODE));

	LoadResourceFolders(path.GetAppendedRootPath(GlobalPath::BOMB_ORG));
	LoadResourceFolders(path.GetAppendedRootPath(GlobalPath::BOMB_RED));
	LoadResourceFolders(path.GetAppendedRootPath(GlobalPath::OPENING));
	LoadResourceFolders(path.GetTileStage1Path());
	LoadResourceFolders(path.GetTileStage1GuidePath());
	LoadResourceFolders(path.GetAppendedRootPath(GlobalPath::ANIM_CRUMBLING_BOX));

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

void ContentsCore::LoadResource(std::string_view _path, std::string_view _append, bool _isRecursive)
{
	UEngineDirectory dir;
	dir.MoveParentToDirectory(_path);

	if (_append.size() != 0)
	{
		dir.Append(_append);
	}

	if (false == dir.IsExists())
	{
		MSGASSERT("리소스 폴더를 찾지 못했습니다: " + dir.GetPathToString());
		return;
	}

	std::vector<UEngineFile> imageFiles = dir.GetAllFile(false);
	for (size_t i = 0; i < imageFiles.size(); i++)
	{
		std::string&& filePath = imageFiles[i].GetPathToString();
		UImageManager::GetInst().Load(filePath);
	}
}

void ContentsCore::LoadResourceFolders(std::string_view _path, std::string_view _append)
{
	UEngineDirectory dir;
	dir.MoveParentToDirectory(_path);

	if (_append.size() != 0)
	{
		dir.Append(_append);
	}

	if (false == dir.IsExists())
	{
		MSGASSERT("리소스 폴더를 찾지 못했습니다: " + dir.GetPathToString());
		return;
	}
	std::string&& folderPath = dir.GetPathToString();
	UImageManager::GetInst().LoadFolder(folderPath);
}