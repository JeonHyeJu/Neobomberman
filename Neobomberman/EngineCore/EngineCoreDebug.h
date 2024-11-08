#pragma once

namespace UEngineDebug
{
	enum class EDebugPosType
	{
		Rect,
		Circle,
	};

	void SetIsDebug(bool _IsDebug);

	void SwitchIsDebug();

	void CoreOutPutString(std::string_view _Text);

	void CoreOutPutString(std::string_view _Text, FVector2D _Pos);

	void CoreDebugRender(FTransform _Trans, EDebugPosType _Type);

	void PrintEngineDebugRender();
}