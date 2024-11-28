#include "PreCompile.h"
#include "EngineSound.h"
#include <EngineBase/EngineString.h>
#include <EngineBase/EngineDebug.h>

std::map<std::string, UEngineSound*> UEngineSound::Sounds;
std::list<USoundPlayer> UEngineSound::Players;

FMOD::System* SoundSystem = nullptr;

#ifdef _DEBUG
#pragma comment(lib, "fmodL_vc.lib")
#else
#pragma comment(lib, "fmod_vc.lib")
#endif

class SoundInit
{
public:
	SoundInit()
	{
		if (FMOD_RESULT::FMOD_OK != FMOD::System_Create(&SoundSystem))
		{
			MSGASSERT("FMOD 시스템 초기화에 실패했습니다.");
			return;
		}

		if (FMOD_RESULT::FMOD_OK != SoundSystem->init(32, FMOD_DEFAULT, nullptr))
		{
			MSGASSERT("FMOD 시스템 초기화에 실패했습니다.");
			return;
		}
	}
};

SoundInit SoundInitObject;

// Temp
void UEngineSound::StopPlayer(std::string_view _Name)
{
	std::list<USoundPlayer>::iterator StartIter = Players.begin();
	std::list<USoundPlayer>::iterator EndIter = Players.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		USoundPlayer& CurSoundPlayer = *StartIter;
		if (CurSoundPlayer.GetName() == _Name)
		{
			CurSoundPlayer.Stop();
			// Will not break. I want all sounds to stop.
		}
	}
}

void UEngineSound::AllSoundStop()
{
	std::list<USoundPlayer>::iterator StartIter = Players.begin();
	std::list<USoundPlayer>::iterator EndIter = Players.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		USoundPlayer& CurSoundPlayer = *StartIter;
		CurSoundPlayer.Stop();
	}
}

void UEngineSound::AllSoundOn()
{
	std::list<USoundPlayer>::iterator StartIter = Players.begin();
	std::list<USoundPlayer>::iterator EndIter = Players.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		USoundPlayer& CurSoundPlayer = *StartIter;
		CurSoundPlayer.On();
	}
}

void UEngineSound::AllSoundOff()
{
	std::list<USoundPlayer>::iterator StartIter = Players.begin();
	std::list<USoundPlayer>::iterator EndIter = Players.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		USoundPlayer& CurSoundPlayer = *StartIter;
		CurSoundPlayer.Off();
	}
}

void UEngineSound::Update()
{
	if (nullptr == SoundSystem)
	{
		return;
	}

	std::list<USoundPlayer>::iterator StartIter = Players.begin();
	std::list<USoundPlayer>::iterator EndIter = Players.end();

	std::string log = "------------------------------------------\n";
	for (; StartIter != EndIter; )
	{
		USoundPlayer& CurSoundPlayer = *StartIter;
		const std::string& name = CurSoundPlayer.GetName();
		log += name + ", ";

		if (true == CurSoundPlayer.IsPlaying())
		{
			++StartIter;
			continue;
		}

		StartIter = Players.erase(StartIter);
	}

	log += "\n";
	OutputDebugString(log.c_str());

	if (FMOD_RESULT::FMOD_OK != SoundSystem->update())
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

	if (nullptr != SoundSystem)
	{
		SoundSystem->release();
		SoundSystem = nullptr;
	}
}

UEngineSound::UEngineSound()
{
}

UEngineSound::~UEngineSound()
{
	if (nullptr != SoundHandle)
	{
		SoundHandle->release();
		SoundHandle = nullptr;
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

	UEngineSound* NewSound = new UEngineSound();

	if (false != UEngineSound::Sounds.contains(UpperString))
	{
		delete NewSound;
		MSGASSERT("이미 로드한 사운드를 또 로드하려고 했습니다." + UpperString);
		return;
	}

	if (false == NewSound->ResLoad(_Path))
	{
		delete NewSound;
		MSGASSERT("사운드 로드에 실패했습니다" + UpperString);
		return;
	}

	UEngineSound::Sounds.insert({ UpperString, NewSound });
}

UEngineSound* UEngineSound::Find(std::string_view _Name)
{
	std::string UpperString = UEngineString::ToUpper(_Name);

	if (false == Sounds.contains(UpperString))
	{
		return nullptr;
	}

	return Sounds[UpperString];
}

USoundPlayer UEngineSound::Play(std::string_view _Name, int _posMs, int _loopCnt, bool _isCheckOverlap)
{
	std::string UpperString = UEngineString::ToUpper(_Name);

	UEngineSound* FindSound = Find(_Name);

	if (nullptr == FindSound)
	{
		MSGASSERT("로드하지 않은 사운드를 재생하려고 했습니다" + UpperString);
	}

	if (_isCheckOverlap)
	{
		std::list<USoundPlayer>::iterator StartIter = Players.begin();
		std::list<USoundPlayer>::iterator EndIter = Players.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			USoundPlayer& CurSoundPlayer = *StartIter;
			if (CurSoundPlayer.GetName() == _Name)
			{
				return CurSoundPlayer;
			}
		}
	}

	FMOD::Channel* Ch = nullptr;

	SoundSystem->playSound(FindSound->SoundHandle, nullptr, false, &Ch);

	if (_posMs > 0)
	{
		Ch->setPosition(_posMs, FMOD_TIMEUNIT_MS);
	}
	Ch->setLoopCount(_loopCnt);
	Ch->setVolume(1.0f);

	USoundPlayer NewPlayer;
	NewPlayer.Control = Ch;
	NewPlayer.SoundHandle = FindSound->SoundHandle;
	NewPlayer.Name = _Name;

	Players.push_back(NewPlayer);

	return NewPlayer;
}

bool UEngineSound::ResLoad(std::string_view _Path)
{
	SoundSystem->createSound(_Path.data(), FMOD_LOOP_NORMAL, nullptr, &SoundHandle);

	if (nullptr == SoundHandle)
	{
		MSGASSERT("사운드 로딩에 실패했습니다" + std::string(_Path));
		return false;
	}

	return true;
}