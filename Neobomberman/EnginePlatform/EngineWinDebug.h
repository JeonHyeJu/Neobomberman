#pragma once
#include <Windows.h>
#include <assert.h>

namespace UEngineDebug
{
	void WinAPIOutPutString(UEngineWinImage* _Image, std::string_view _Text, FVector2D _Pos);
}
