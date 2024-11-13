#pragma once
#include <string>
#include <vector>
#include <EngineBase/EngineMath.h>

template <typename T>
void DebugPrintEnumVector(const std::vector<T>& _vec, std::string_view name = "")
{
	std::string temp = name.data() + std::string(" : ");
	for (size_t i = 0, size = _vec.size(); i < size; ++i)
	{
		temp += "(" + std::to_string(static_cast<int>(_vec[i])) + "), ";
	}
	temp += "\n";
	OutputDebugString(temp.c_str());
}

void DebugPrintFIntVector(const std::vector<FIntPoint>& _vec, std::string_view name = "")
{
	std::string temp = name.data() + std::string(" : ");
	for (size_t i = 0, size = _vec.size(); i < size; ++i)
	{
		temp += "(" + std::to_string(_vec[i].Y) + "," + std::to_string(_vec[i].X) + "), ";
	}
	temp += "\n";
	OutputDebugString(temp.c_str());
}

void DebugPrintFIntVector(std::list<FIntPoint>& _vec, std::string_view name = "")
{
	std::list<FIntPoint>::iterator it = _vec.begin();
	std::list<FIntPoint>::iterator itEnd = _vec.end();
	std::string temp = name.data() + std::string(" : ");
	for (it; it != itEnd; ++it)
	{
		temp += "(" + std::to_string((*it).Y) + "," + std::to_string((*it).X) + "), ";
	}
	temp += "\n";
	OutputDebugString(temp.c_str());
}