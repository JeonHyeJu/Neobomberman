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
	void LoadResourceNotRecursively(std::string_view _path, std::string_view _append="");
	void LoadResourceFolders(std::string_view _path, std::string_view _append="");

	ULevel* TitleLevel = nullptr;
};
