#include "PreCompile.h"
#include "PlayGameMode.h"

#include <EngineCore/Level.h>
#include "PlayMap.h"

APlayGameMode::APlayGameMode()
{
}

APlayGameMode::~APlayGameMode()
{
}

void APlayGameMode::BeginPlay()
{
	// GetWorld()->SpawnActor<BackGround>();

	APlayMap* PMAP0 = GetWorld()->SpawnActor<APlayMap>();
}
