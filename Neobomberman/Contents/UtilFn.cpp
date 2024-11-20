#include "PreCompile.h"
#include "UtilFn.h"
#include <cmath>

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