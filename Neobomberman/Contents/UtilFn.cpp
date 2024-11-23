#include "PreCompile.h"
#include "UtilFn.h"
#include <EngineBase/EngineDirectory.h>
#include <EngineBase/EngineFile.h>
#include <EngineBase/EngineSerializer.h>
#include <cmath>

int UtilFn::VectorToInt(const std::vector<int>& _vec)
{
	int ret = 0;
	for (size_t i = 0, size = _vec.size(); i < size; ++i)
	{
		int exp = static_cast<int>(size - i - 1);
		ret += _vec[i] * static_cast<int>(pow(10, exp));
	}

	return ret;
}

std::vector<int> UtilFn::IntToVector(int _val, int _maxSize)
{
	std::vector<int> vec(_maxSize, -1);
	vec.resize(_maxSize);	// MAX

	if (_val == 0)
	{
		vec[vec.size() - 1] = 0;
		return vec;
	}

	static int CurNumCnt = 0;

	int total = _val;
	int numCnt = 0;

	// Total 600
	// 600 / 10 -> 60
	// 60 / 10 -> 6
	// 6 / 10 -> 0
	while (total != 0)
	{
		total /= 10;
		numCnt++;
	}

	// 652 / 100 = 6, rest: 52
	// 52 / 10 = 5, rest 2
	// 2 / 1 = 2, rest 0
	total = _val;
	for (int i = numCnt - 1; i >= 0; --i)
	{
		int denorm = static_cast<int>(std::pow(10, i));
		int val = total / denorm;
		vec[_maxSize - i - 1] = val;
		total %= denorm;
	}

	return vec;
}

std::vector<int> UtilFn::LoadIntVecStr(std::string_view _path, std::string_view _fileName)
{
	std::vector<int> retVec;
	retVec.reserve(MAX_INT_VEC_SIZE);

	UEngineDirectory dir;
	if (dir.MoveRelative(_path) == false) return retVec;

	UEngineSerializer serializer;
	UEngineFile file = dir.GetPathToString() + "\\" + _fileName.data();
	file.FileOpen("rb");
	file.Read(serializer);

	std::vector<int> singleNum;
	singleNum.reserve(10);
	bool isNegative = false;

	while (true)
	{
		char c = NULL;

		serializer >> c;

		if (c == '[' || c == ']' || c == ' ')
		{
			continue;
		}
		else if (c == ',')
		{
			int val = VectorToInt(singleNum);
			if (val == 11)
			{
				int a = 0;
			}
			if (isNegative)
			{
				val *= -1;
			}
			retVec.push_back(val);

			isNegative = false;
			singleNum.clear();
		}
		else if (c == '-')
		{
			isNegative = true;
		}
		else
		{
			singleNum.push_back(c - '0');
		}

		if (serializer.IsEndOfData())
		{
			if (singleNum.size() > 0)
			{
				int val = VectorToInt(singleNum);
				if (val == 11)
				{
					int a = 0;
				}
				if (isNegative)
				{
					val *= -1;
				}
				retVec.push_back(val);
			}

			break;
		}
	}
	file.Close();

	return retVec;
}