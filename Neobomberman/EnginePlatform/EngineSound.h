#pragma once
#include <EngineBase/EngineResources.h>
#include <map>

#include "ThirdParty/FMOD/inc/fmod.hpp"
#include "math.h"

class USoundPlayer 
{
public:
	friend class UEngineSound;

	~USoundPlayer()
	{
	}

	void On()
	{
		Control->setPaused(false);
	}

	void Off()
	{
		Control->setPaused(true);
	}

	void Stop()
	{
		Control->stop();
		Control = nullptr;
	}

	bool IsPlaying()
	{
		bool Check = true;
		Control->isPlaying(&Check);
		return Check;
	}

	void SetVolume(float _Volume)
	{
		_Volume = UEngineMath::Clamp(_Volume, 0.0f, 1.0f);

		Control->setVolume(_Volume);
	}

	void OnOffSwtich()
	{
		bool Check = false;
		Control->getPaused(&Check);

		if (true == Check)
		{
			Control->setPaused(false);
		}
		else 
		{
			Control->setPaused(true);
		}
	}

	void SetPosition(unsigned int _Value)
	{
		Control->setPosition(_Value, FMOD_TIMEUNIT_MS);
	}

	void Loop(int Count = -1)
	{
		Control->setLoopCount(Count);
	}

	void ReStart()
	{
		SetPosition(0);
	}

	unsigned int LengthMs()
	{
		unsigned int ResultLength = 0;
		SoundHandle->getLength(&ResultLength, FMOD_TIMEUNIT_MS);
		return ResultLength;
	}

	inline const std::string& GetName() const
	{
		return Name;
	}

private:
	std::string Name = "";
	FMOD::Channel* Control = nullptr;
	FMOD::Sound* SoundHandle = nullptr;;
};

class UEngineSound : public UEngineResources
{
public:
	UEngineSound();
	~UEngineSound();

	UEngineSound(const UEngineSound& _Other) = delete;
	UEngineSound(UEngineSound&& _Other) noexcept = delete;
	UEngineSound& operator=(const UEngineSound& _Other) = delete;
	UEngineSound& operator=(UEngineSound&& _Other) noexcept = delete;

	static void Load(std::string_view _Path);
	static void Load(std::string_view _Name, std::string_view _Path);
	static USoundPlayer Play(std::string_view _Name, int _posMs=-1, int _loopCnt=0, bool _isCheckOverlap=true);
	static UEngineSound* Find(std::string_view _Name);
	static void StopPlayer(std::string_view _Name);		// Temp
	static void Release();

	static void Update();

	static void AllSoundStop();
	static void AllSoundOff();
	static void AllSoundOn();

protected:

private:
	static std::map<std::string, UEngineSound*> Sounds;
	static std::list<USoundPlayer> Players;

	FMOD::Sound* SoundHandle = nullptr;

	bool ResLoad(std::string_view _Path);
};

