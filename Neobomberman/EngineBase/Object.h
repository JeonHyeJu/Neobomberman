#pragma once
#include <string>

class UObject
{
public:
	UObject();

	virtual ~UObject();

	UObject(const UObject& _Other) = delete;
	UObject(UObject&& _Other) noexcept = delete;
	UObject& operator=(const UObject& _Other) = delete;
	UObject& operator=(UObject&& _Other) noexcept = delete;

	virtual void SetName(std::string_view _Name)
	{
		Name = _Name.data();
	}

protected:

private:
	std::string Name;

};

