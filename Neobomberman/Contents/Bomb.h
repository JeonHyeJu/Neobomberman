#pragma once

class Bomb
{
public:
	Bomb();
	~Bomb();

	Bomb(const Bomb& _other) = delete;
	Bomb(Bomb&& _other) = delete;
	Bomb& operator=(const Bomb& _other) = delete;
	Bomb& operator=(Bomb&& _other) noexcept = delete;

	void CreateTimer();
	void DeleteTimer();

	// Get, Set
	inline int GetPower() const
	{
		return mPower;
	}
	inline void SetPower(int _val)
	{
		mPower = _val;
	}

private:
	void Explode();

	const int EXPLODE_MILLI_SECONDS = 3000;

	class Timer* mTimer = nullptr;
	int mPower = 1;
};
