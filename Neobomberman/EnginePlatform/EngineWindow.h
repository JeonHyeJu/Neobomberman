#pragma once
// os Header
#include <Windows.h>

// std Header
#include <map>
#include <string>
#include <functional>

// user Header
#include <EngineBase/EngineDelegate.h>


class UEngineWindow
{
public:
	static void EngineWindowInit(HINSTANCE _Instance);
	static void CreateWindowClass(const WNDCLASSEXA& _Class);

	static int WindowMessageLoop(EngineDelegate _StartFunction, EngineDelegate _FrameFunction);
	// constrcuter destructer
	UEngineWindow();
	~UEngineWindow();

	// delete Function
	UEngineWindow(const UEngineWindow& _Other) = delete;
	UEngineWindow(UEngineWindow&& _Other) noexcept = delete;
	UEngineWindow& operator=(const UEngineWindow& _Other) = delete;
	UEngineWindow& operator=(UEngineWindow&& _Other) noexcept = delete;

	void Create(std::string_view _TitleName, std::string_view _ClassName = "Default");
	void Open(std::string_view _TitleName = "Window");

	inline HDC GetBackBuffer()
	{
		return BackBuffer;
	}



protected:

private:
	static HINSTANCE hInstance;
	static std::map<std::string, WNDCLASSEXA> WindowClasss;

	HDC BackBuffer = nullptr;
	HWND WindowHandle = nullptr;

};

