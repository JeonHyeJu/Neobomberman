#pragma once
#include "Object.h"
#include "EnginePath.h"

class UEngineResources : public UObject
{
public:
	UEngineResources();
	~UEngineResources();

	UEngineResources(const UEngineResources& _Other) = delete;
	UEngineResources(UEngineResources&& _Other) noexcept = delete;
	UEngineResources& operator=(const UEngineResources& _Other) = delete;
	UEngineResources& operator=(UEngineResources&& _Other) noexcept = delete;

protected:

private:
	UEnginePath Path;
};

