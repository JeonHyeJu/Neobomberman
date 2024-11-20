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

	inline unsigned __int8 GetPlayer1Life() const
	{
		return Player1Life;
	}
	unsigned __int8 AddPlayer1Life(__int8 _val)
	{
		Player1Life += _val;
		if (Player1Life > MAX_PLAYER_LIFE)
		{
			Player1Life = MAX_PLAYER_LIFE;
		}
		return Player1Life;
	}

	const unsigned __int8 MAX_PLAYER_LIFE = 2;
	const unsigned __int8 MAX_COIN = 99;

private:
	unsigned __int8 Coin = 0;
	unsigned __int8 Player1Life = 2;
};