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
		if (_Max < _Min)
		{
			int SwapValue = _Min;
			_Min = _Max;
			_Max = SwapValue;
		}

		// _Min ~ _Max ·£´ý°ª »Ì¾ÆÁà
		std::uniform_int_distribution<int> RandomCreate(_Min, _Max);

		return RandomCreate.operator()(MtGen);
	}

	std::vector<int> RandomIntMultiple(int _Min, int _Max, int _cnt)
	{
		if (_Max < _Min)
		{
			int SwapValue = _Min;
			_Min = _Max;
			_Max = SwapValue;
		}

		std::vector<int> values;
		values.reserve(_cnt);

		if ((_Max - _Min) < _cnt)
		{
			return values;
		}
		
		while (values.size() < _cnt)
		{
			std::uniform_int_distribution<int> RandomCreate(_Min, _Max);
			int val = RandomCreate(MtGen);

			bool isSame = false;
			for (size_t i = 0, size = values.size(); i < size; ++i)
			{
				if (static_cast<int>(values[i]) == val)
				{
					isSame = true;
					break;
				}
			}

			if (!isSame)
			{
				values.push_back(val);
			}
		}

		return values;
	}

	float Randomfloat(float _Min, float _Max)
	{
		if (_Max < _Min)
		{
			float SwapValue = _Min;
			_Min = _Max;
			_Max = SwapValue;
		}

		// _Min ~ _Max ·£´ý°ª »Ì¾ÆÁà
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