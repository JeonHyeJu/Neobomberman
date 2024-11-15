#pragma once
#include <random>

class UEngineRandom
{
public:
	UEngineRandom();
	~UEngineRandom();

	UEngineRandom(const UEngineRandom& _Other) = delete;
	UEngineRandom(UEngineRandom&& _Other) noexcept = delete;
	UEngineRandom& operator=(const UEngineRandom& _Other) = delete;
	UEngineRandom& operator=(UEngineRandom&& _Other) noexcept = delete;

	void SetSeed(__int64 _Seed)
	{
		MtGen = std::mt19937_64(_Seed);
	}

	int RandomInt(int _Min, int _Max)
	{
		std::uniform_int_distribution<int> RandomCreate(_Min, _Max);

		return RandomCreate.operator()(MtGen);
	}

	float Randomfloat(float _Min, float _Max)
	{
		std::uniform_real_distribution<float> RandomCreate(_Min, _Max);
		return RandomCreate.operator()(MtGen);
	}


protected:

private:
	std::mt19937_64 MtGen = std::mt19937_64(time(nullptr));
};

//class RoomMapCreator : public MapCreator
//{
//	virtual void MapCreate()
//	{
//
//	}
//};