#include "PreCompile.h"
#include <EngineCore/EngineAPICore.h>

#pragma comment (lib, "EngineCore.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	return UEngineAPICore::EngineStart(hInstance);
}