#pragma once
#include <EngineBase/EngineResources.h>
#include <map>

#include "ThirdParty/FMOD/inc/fmod.hpp"

class USoundPlayer
{
public:
	friend class UEngineSound;

	void On()
	{
		Control->setPaused(false);
	}

	void Off()
	{
		Control->setPaused(true);
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

	void Loop(int Count = -1)
	{
		Control->setLoopCount(Count);
	}

private:
	FMOD::Channel* Control = nullptr;
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

	static void InitFMOD();

	static USoundPlayer Play(std::string_view _Name);

	static void Load(std::string_view _Path);
	static void Load(std::string_view _Name, std::string_view _Path);
	static UEngineSound* Find(std::string_view _Name);
	static void Release();
	static void Update();

protected:

private:
	bool LoadResource(std::string_view _Path);

	static std::map<std::string, UEngineSound*> Sounds;
	FMOD::Sound* SoundHandle = nullptr;
};

