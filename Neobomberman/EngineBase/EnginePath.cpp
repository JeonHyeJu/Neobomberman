#include "PreCompile.h"
#include "EnginePath.h"
#include "EngineDebug.h"

UEnginePath::UEnginePath()
	: Path(std::filesystem::current_path())
{
}

UEnginePath::UEnginePath(std::string_view _Path)
	: Path(_Path)
{

}

UEnginePath::UEnginePath(std::filesystem::path _Path)
	: Path(_Path)
{

}

UEnginePath::~UEnginePath()
{
}

std::string UEnginePath::GetPathToString()
{
	return Path.string();
}

std::string UEnginePath::GetFileName()
{
	if (true == IsDirectory())
	{
		MSGASSERT("파일 경로 일때만 GetFileName을 호출할수 있습니다." + Path.string());
		return "";
	}

	return Path.filename().string();
}

std::string UEnginePath::GetDirectoryName()
{
	if (false == IsDirectory())
	{
		MSGASSERT("디렉토리 경로 일때만 GetDirectoryName을 호출할수 있습니다." + Path.string());
		return "";
	}

	return Path.filename().string();
}

std::string UEnginePath::GetExtension()
{
	return Path.extension().string();
}


bool UEnginePath::IsExists()
{
	return std::filesystem::exists(Path);
}

bool UEnginePath::IsFile()
{
	return false == IsDirectory();
}

bool UEnginePath::IsDirectory()
{
	return std::filesystem::is_directory(Path);
}

void UEnginePath::MoveParent()
{
	Path = Path.parent_path();
}

void UEnginePath::Append(std::string_view _AppendName)
{
	Path.append(_AppendName);
}

bool UEnginePath::MoveParentToDirectory(std::string_view _Path)
{
	UEnginePath DummyPath = UEnginePath(Path);

	if (false == DummyPath.IsDirectory())
	{
		MSGASSERT("디렉토리 경로일때만 MoveParentToDirectory 를 호출할수 있습니다");
		return false;
	}

	bool Result = false;
	std::filesystem::path CurPath = DummyPath.Path;
	std::filesystem::path Root = CurPath.root_path();

	while (true)
	{
		CurPath = DummyPath.Path;

		if (CurPath == Root)
		{
			break;
		}

		CurPath.append(_Path);
		if (true == std::filesystem::exists(CurPath))
		{
			Result = true;
			Path = CurPath;
			break;
		}
		DummyPath.MoveParent();
	}

	return Result;
}

bool UEnginePath::MoveRelative(std::string_view _path, char delimiter)
{
	if (_path.size() == 0) return false;

	std::vector<std::string> paths;			// relative paths
	paths.reserve(128);						// Temp

	std::string findPath = _path.data();
	size_t idx = findPath.find(delimiter);
	while (idx != findPath.npos)
	{
		std::string subStr = findPath.substr(0, idx);
		paths.push_back(subStr.data());

		findPath = findPath.substr(idx + 1, -1);
		idx = findPath.find(delimiter);
	}
	if (findPath.size() != 0)
	{
		paths.push_back(findPath);
	}

	if (paths.size() == 0) return false;

	for (size_t i = 0; i < paths.size(); ++i)
	{
		if (i == 0)
		{
			if (MoveParentToDirectory(paths[i]) == false)
			{
				MSGASSERT("상대경로를 찾지 못했습니다.");
				return false;
			}
		}
		else
		{
			Append(paths[i]);
		}
	}

	return true;
}