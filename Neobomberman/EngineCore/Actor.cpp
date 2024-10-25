#include "PreCompile.h"
#include "Actor.h"
#include <EngineCore/EngineAPICore.h>

AActor::AActor()
{
}

AActor::~AActor()
{
}


void AActor::Render()
{
	FVector2D LeftTop = Location - Scale.Half();
	FVector2D RightBot = Location + Scale.Half();


	UEngineWindow& MainWindow = UEngineAPICore::GetCore()->GetMainWindow();
	HDC BackHDC = MainWindow.GetBackBuffer();

	Rectangle(BackHDC, LeftTop.iX(), LeftTop.iY(), RightBot.iX(), RightBot.iY());
}