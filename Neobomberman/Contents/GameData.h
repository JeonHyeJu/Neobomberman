#pragma once
class GameData
{
public:
	GameData() {}
	~GameData() {}

	GameData(const GameData& _Other) = delete;
	GameData(GameData&& _Other) noexcept = delete;
	GameData& operator=(const GameData& _Other) = delete;
	GameData& operator=(GameData&& _Other) noexcept = delete;

	static GameData& GetInstance()
	{
		static GameData instance;
		return instance;
	}

	inline unsigned __int8 GetCoin() const
	{
		return Coin;
	}
	unsigned __int8 AddCoin(__int8 _val)
	{
		Coin += _val;
		if (Coin > MAX_COIN)
		{
			Coin = MAX_COIN;
		}
		return Coin;
	}

	inline int GetPlayer1Life() const
	{
		return Player1Life;
	}
	inline int GetPlayer2Life() const
	{
		return Player2Life;
	}
	int AddPlayer1Life(__int8 _val)
	{
		Player1Life += _val;
		if (Player1Life > MAX_PLAYER_LIFE)
		{
			Player1Life = MAX_PLAYER_LIFE;
		}
		return Player1Life;
	}
	int AddPlayer2Life(__int8 _val)
	{
		Player2Life += _val;
		if (Player2Life > MAX_PLAYER_LIFE)
		{
			Player2Life = MAX_PLAYER_LIFE;
		}
		return Player2Life;
	}

	inline int GetPlayer1Score() const
	{
		return Player1Score;
	}
	int AddPlayer1Score(int _val)
	{
		Player1Score += _val;
		return Player1Score;
	}
	int SetPlayer1Score(int _val)
	{
		Player1Score = _val;
		return Player1Score;
	}

	void ResetScore()
	{
		Player1Score = 0;
	}

	inline int GetPlayer1BombPower() const
	{
		return Player1BombPower;
	}

	inline float GetPlayer1Speed() const
	{
		return Player1Speed;
	}

	inline int GetPlayer1BombCnt() const
	{
		return Player1BombCnt;
	}

	void SetPlayer1BombPower(int _power)
	{
		Player1BombPower = _power;
	}

	void SetPlayer1Speed(float _speed)
	{
		Player1Speed = _speed;
	}

	void SetPlayer1BombCnt(int _cnt)
	{
		Player1BombCnt = _cnt;
	}

	const unsigned __int8 MAX_PLAYER_LIFE = 2;
	const unsigned __int8 MAX_COIN = 99;

private:
	unsigned __int8 Coin = 0;
	int Player1Life = MAX_PLAYER_LIFE;
	int Player2Life = MAX_PLAYER_LIFE;
	int Player1Score = 0;

	float Player1Speed = 1;
	int Player1BombCnt = 1;
	int Player1BombPower = 2;
};