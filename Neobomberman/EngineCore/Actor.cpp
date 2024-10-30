#include "PreCompile.h"
#include "Actor.h"
#include <EngineCore/EngineAPICore.h>
#include <EngineBase/EngineMath.h>
#include <EnginePlatform/EngineWindow.h>
#include <EnginePlatform/EngineWinImage.h>

AActor::AActor()
{
}

AActor::~AActor()
{
}


void AActor::Render()
{
	FVector2D LeftTop = Transform.Location - Transform.Scale.Half();
	FVector2D RightBot = Transform.Location + Transform.Scale.Half();


	UEngineWindow& MainWindow = UEngineAPICore::GetCore()->GetMainWindow();
	UEngineWinImage* BackBufferImage = MainWindow.GetBackBuffer();
	HDC BackBufferDC = BackBufferImage->GetDC();
	Rectangle(BackBufferDC, LeftTop.iX(), LeftTop.iY(), RightBot.iX(), RightBot.iY());
}