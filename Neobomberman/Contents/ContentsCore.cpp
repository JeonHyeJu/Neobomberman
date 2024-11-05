#include "PreCompile.h"
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineDebug.h>
#include <EngineBase/EngineFile.h>
#include <EngineCore/ImageManager.h>
#include "ContentsCore.h"
#include "TitleGameMode.h"
#include "PlayGameMode.h"
#include "Player.h"
#include "GlobalVar.h"

// Temp. TODO: remove
#include "TileMapGameMode.h"

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

	/** Temp.For tilemap. **/
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
	UImageManager& imageManager = UImageManager::GetInst();

	/** Load resources **/
	LoadResource("Resources", "", false);
	LoadResource("Resources", "Background", false);
	LoadResource("Resources", "Character_32x64", false);
	LoadResource("Resources", "Riding_32x32", false);
	LoadResource("Resources", "Enemies_32x32", false);

	LoadResourceFolders("Resources", "BombOrg_16x16");
	LoadResourceFolders("Resources", "BombRed_16x16");
	LoadResourceFolders("Resources", "Opening");
	LoadResourceFolders("Resources\\Tiles\\TileStage_1");
	LoadResourceFolders("Resources\\Tiles\\TileStage_1_Guide");

	/** Cutting **/
	imageManager.CuttingSprite("MainCharater_White.png", GlobalVar::BOMBERMAN_SIZE);
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