#pragma once
#include <EngineBase/EngineDelegate.h>
#include <EngineBase/EngineDebug.h>

#include <vector>
#include <functional>

enum class KeyEvent
{
	Down,
	Press,
	Free,
	Up,
};

class UEngineInput
{
public:
	~UEngineInput();

	UEngineInput(const UEngineInput& _Other) = delete;
	UEngineInput(UEngineInput&& _Other) noexcept = delete;
	UEngineInput& operator=(const UEngineInput& _Other) = delete;
	UEngineInput& operator=(UEngineInput&& _Other) noexcept = delete;

	static UEngineInput& GetInst()
	{
		static UEngineInput Inst = UEngineInput();
		return Inst;
	}

private:
	class UEngineKey
	{
	public:
		UEngineKey() {}
		UEngineKey(int _Key): Key(_Key) {}

		void EventCheck(float _DeltaTime);
		void KeyCheck(float _DeltaTime);

		int Key = -1;
		
		bool IsDown = false;	// released -> pressed
		bool IsPress = false;	// pressed -> pressed
		bool IsUp = false;		// pressed -> released
		bool IsFree = true;		// released -> released

		float PressTime = 0.0f;

		std::vector<std::function<void(float)>> PressEvents;
		std::vector<std::function<void(float)>> DownEvents;
		std::vector<std::function<void(float)>> UpEvents;
		std::vector<std::function<void(float)>> FreeEvents;
	};

public:
	void KeyCheck(float _DeltaTime);
	void EventCheck(float _DeltaTime);

	bool IsDown(int _KeyIndex)
	{
		if (false == Keys.contains(_KeyIndex))
		{
			MSGASSERT("아직도 등록되지 않은 키가 존재합니다.");
			return false;
		}

		return Keys[_KeyIndex].IsDown;
	}

	bool IsUp(int _KeyIndex)
	{
		if (false == Keys.contains(_KeyIndex))
		{
			MSGASSERT("아직도 등록되지 않은 키가 존재합니다.");
			return false;
		}

		return Keys[_KeyIndex].IsUp;
	}

	bool IsPress(int _KeyIndex)
	{
		if (false == Keys.contains(_KeyIndex))
		{
			MSGASSERT("아직도 등록되지 않은 키가 존재합니다.");
			return false;
		}

		return Keys[_KeyIndex].IsPress;
	}

	float GetPressTime(int _KeyIndex)
	{
		if (false == Keys.contains(_KeyIndex))
		{
			MSGASSERT("아직도 등록되지 않은 키가 존재합니다.");
			return false;
		}

		return Keys[_KeyIndex].PressTime;
	}

	bool IsFree(int _KeyIndex)
	{
		if (false == Keys.contains(_KeyIndex))
		{
			MSGASSERT("아직도 등록되지 않은 키가 존재합니다.");
			return false;
		}

		return Keys[_KeyIndex].IsFree;
	}

	void BindAction(int _KeyIndex, KeyEvent _EventType, std::function<void(float)> _Function);

protected:

private:
	UEngineInput();

	std::map<int, UEngineKey> Keys;
};

