#pragma once
#include <Windows.h>

class UEngineTimer
{
public:
	UEngineTimer();
	~UEngineTimer();

	UEngineTimer(const UEngineTimer& _Other) = delete;
	UEngineTimer(UEngineTimer&& _Other) noexcept = delete;
	UEngineTimer& operator=(const UEngineTimer& _Other) = delete;
	UEngineTimer& operator=(UEngineTimer&& _Other) noexcept = delete;

	void TimeCheck();

	float GetDeltaTime()
	{
		return fDeltaTime;
	}

	double GetDoubleDeltaTime()
	{
		return DeltaTime;
	}

	void TimeStart();

	float End();

	double DEnd();

protected:

private:

	LARGE_INTEGER Count = LARGE_INTEGER();

	LARGE_INTEGER PrevTime = LARGE_INTEGER();

	LARGE_INTEGER CurTime = LARGE_INTEGER();

	double TimeCounter = 0.0f;

	double DeltaTime = 0.0;

	float fDeltaTime = 0.0f;
};

