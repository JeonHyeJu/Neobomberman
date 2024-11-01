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
	pCore->GetMainWindow().SetWindowPosAndScale({ 0, 0 }, { 302, 224 });	// You must call this.

	LoadResources();

	pCore->CreateLevel<APlayGameMode, APlayer>("Play");
	pCore->CreateLevel<ATitleGameMode, AActor>("Title");

	pCore->OpenLevel("Title");
}

void ContentsCore::Tick()
{
	// Nothing..
}

void ContentsCore::LoadResources()
{
	UImageManager& imageManager = UImageManager::GetInst();

	/** Load resources **/
	UEngineDirectory resourceDir;
	if (false == resourceDir.MoveParentToDirectory("Resources"))
	{
		MSGASSERT("리소스 폴더를 찾지 못했습니다.");
		return;
	}

	std::vector<UEngineFile> imageFiles = resourceDir.GetAllFile();
	for (size_t i = 0; i < imageFiles.size(); i++)
	{
		std::string&& filePath = imageFiles[i].GetPathToString();
		imageManager.Load(filePath);
	}

	//{
	//	UEngineDirectory BombDir;
	//	BombDir.MoveParentToDirectory("Resources");
	//	BombDir.Append("BombOrg_16x16");
	//	std::string&& folderPath = BombDir.GetPathToString();
	//	imageManager.LoadFolder(folderPath);
	//}
	//
	//{
	//	UEngineDirectory OpeningDir;
	//	OpeningDir.MoveParentToDirectory("Resources");
	//	OpeningDir.Append("Opening");
	//	std::string&& folderPath = OpeningDir.GetPathToString();
	//	imageManager.LoadFolder(folderPath);
	//}

	/** Cutting **/
	imageManager.CuttingSprite("MainCharater_White.png", GlobalVar::BOMBERMAN_SIZE);
}