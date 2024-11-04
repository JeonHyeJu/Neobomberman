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
	pCore->GetMainWindow().SetWindowPosAndScale({ 0, 0 }, GlobalVar::WINDOW_SIZE);	// You must call this.

	InitResources();

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
	LoadResourceNotRecursively("Resources");
	LoadResourceNotRecursively("Resources", "Background");
	LoadResourceNotRecursively("Resources", "Riding_32x32");
	LoadResourceNotRecursively("Resources", "Enemies_32x32");

	LoadResourceFolders("Resources", "BombOrg_16x16");
	LoadResourceFolders("Resources", "BombRed_16x16");
	LoadResourceFolders("Resources", "Opening");

	/** Cutting **/
	imageManager.CuttingSprite("MainCharater_White.png", GlobalVar::BOMBERMAN_SIZE);
}

void ContentsCore::LoadResourceNotRecursively(std::string_view _path, std::string_view _append)
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