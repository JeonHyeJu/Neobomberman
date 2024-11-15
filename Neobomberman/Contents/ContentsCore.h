#pragma once
#include <EngineCore/EngineAPICore.h>
#include <EnginePlatform/EngineWindow.h>

class ContentsCore : public UContentsCore
{
public:
	ContentsCore();
	~ContentsCore();

	ContentsCore(const ContentsCore& _other) = delete;
	ContentsCore(ContentsCore&& _other) noexcept = delete;
	ContentsCore& operator=(const ContentsCore& _other) = delete;
	ContentsCore& operator=(ContentsCore&& _other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick() override;

private:
	void InitResources();
	void LoadImages(std::string_view _path);
	void LoadImageFolders(std::string_view _path, std::string_view _append="");
	void LoadSounds(std::string_view _path);

	ULevel* TitleLevel = nullptr;
};
