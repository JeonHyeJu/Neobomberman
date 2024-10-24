#include "PreCompile.h"
#include "Bomb.h"
#include "Timer.h"

#ifdef _DEBUG
#include <iostream>
#endif

Bomb::Bomb()
{
	
}

Bomb::~Bomb()
{
	DeleteTimer();
}

void Bomb::CreateTimer()
{
	DeleteTimer();

	mTimer = new Timer;
	mTimer->SetTimeout(&Bomb::Explode, this, EXPLODE_MILLI_SECONDS);
	//mTimer->SetTimeout([this]() { Explode(); }, EXPLODE_MILLI_SECONDS);
}

void Bomb::DeleteTimer()
{
	if (mTimer)
	{
		mTimer->Stop();

		delete mTimer;
		mTimer = nullptr;
	}
}

void Bomb::Explode()
{
#ifdef _DEBUG
	std::cout << "Bomb::Explode()" << std::endl;
#endif
}