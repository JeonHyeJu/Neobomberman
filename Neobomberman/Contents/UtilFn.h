#pragma once
#include <vector>

namespace UtilFn
{
	const int MAX_INT_VEC_SIZE = 1024;

	extern int VectorToInt(const std::vector<int>& _vec);
	extern std::vector<int> IntToVector(int _val, int _maxSize=10);
	extern std::vector<int> LoadIntVecStr(std::string_view _path, std::string_view _fileName);
}
