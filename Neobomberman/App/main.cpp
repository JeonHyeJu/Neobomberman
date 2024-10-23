#include <Windows.h>
#include <EnginePlatform/EngineWindow.h>
#include "Contents/Bomb.h"

#pragma comment (lib, "EnginePlatform.lib")
#pragma comment (lib, "Contents.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Bomb bomb;
	bomb.CreateTimer();

	// UGameEngine::Start(hInstance);

	UEngineWindow::EngineWindowInit(hInstance);

	UEngineWindow NewWindow;
	NewWindow.Open();

	return UEngineWindow::WindowMessageLoop();
}