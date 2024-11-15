#include "PreCompile.h"
#include "EngineSound.h"
#include <EngineBase/EngineString.h>
#include <EngineBase/EngineDebug.h>

std::map<std::string, UEngineSound*> UEngineSound::Sounds;
FMOD::System* gSoundSystem = nullptr;

#ifdef _DEBUG
#pragma comment(lib, "fmodL_vc.lib")
#else
#pragma comment(lib, "fmod_vc.lib")
#endif

UEngineSound::UEngineSound()
{
}

UEngineSound::~UEngineSound()
{
}

// static function
void UEngineSound::InitFMOD()
{
	if (FMOD::System_Create(&gSoundSystem) != FMOD_RESULT::FMOD_OK)
	{
		MSGASSERT("FMOD 시스템 초기화에 실패했습니다.");
		return;
	}

	if (gSoundSystem->init(32, FMOD_DEFAULT, nullptr) != FMOD_RESULT::FMOD_OK)
	{
		MSGASSERT("FMOD 시스템 초기화에 실패했습니다.");
		return;
	}
}

void UEngineSound::Update()
{
	if (nullptr == gSoundSystem)
	{
		return;
	}

	if (FMOD_RESULT::FMOD_OK != gSoundSystem->update())
	{
		MSGASSERT("FMOD 시스템 업데이트에 이상이 감지되었습니다.");
	}
}

void UEngineSound::Release()
{
	std::map<std::string, UEngineSound*>::iterator StartIter = Sounds.begin();
	std::map<std::string, UEngineSound*>::iterator EndIter = Sounds.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if (nullptr != StartIter->second)
		{
			delete StartIter->second;
			StartIter->second = nullptr;
		}
	}

	Sounds.clear();

	if (nullptr != gSoundSystem)
	{
		gSoundSystem->release();
		gSoundSystem = nullptr;
	}
}

void UEngineSound::Load(std::string_view _Path)
{
	UEnginePath EnginePath = UEnginePath(_Path);
	std::string FileName = EnginePath.GetFileName();

	UEngineSound::Load(FileName.data(), _Path);
}

void UEngineSound::Load(std::string_view _Name, std::string_view _Path)
{
	std::string UpperString = UEngineString::ToUpper(_Name);
	UEngineSound* NewSound = new UEngineSound;

	if (NewSound->LoadResource(_Path) == false)
	{
		delete NewSound;
		MSGASSERT("사운드 로드에 실패했습니다" + UpperString);
	}

	UEngineSound::Sounds.insert({ UpperString, NewSound });
}

UEngineSound* UEngineSound::Find(std::string_view _Name)
{
	std::string UpperString = UEngineString::ToUpper(_Name);
	if (Sounds.contains(UpperString) == false)
	{
		return nullptr;
	}

	return Sounds[UpperString];
}

USoundPlayer UEngineSound::Play(std::string_view _Name)
{
	std::string UpperString = UEngineString::ToUpper(_Name);
	UEngineSound* FindSound = Find(_Name);

	if (FindSound == nullptr)
	{
		MSGASSERT("로드하지 않은 사운드를 재생하려고 했습니다" + UpperString);
	}

	FMOD::Channel* pChannel = nullptr;
	gSoundSystem->playSound(FindSound->SoundHandle, nullptr, true, &pChannel);
	pChannel->setLoopCount(0);
	pChannel->setVolume(1.0f);

	USoundPlayer NewPlayer;
	NewPlayer.Control = pChannel;
	return NewPlayer;
}

bool UEngineSound::LoadResource(std::string_view _Path)
{
	gSoundSystem->createSound(_Path.data(), FMOD_LOOP_NORMAL, nullptr, &SoundHandle);

	if (SoundHandle == nullptr)
	{
		MSGASSERT("사운드 로딩에 실패했습니다" + std::string(_Path));
		return false;
	}

	return true;
}