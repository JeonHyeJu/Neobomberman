#pragma once
#include <functional>
#include <map>
#include "EngineDebug.h"

class UFSMStateManager
{
public:
	UFSMStateManager()	{}
	~UFSMStateManager()	{}

	UFSMStateManager(const UFSMStateManager& _Other) = delete;
	UFSMStateManager(UFSMStateManager&& _Other) noexcept = delete;
	UFSMStateManager& operator=(const UFSMStateManager& _Other) = delete;
	UFSMStateManager& operator=(UFSMStateManager&& _Other) noexcept = delete;

	class FSMFunction
	{
	public:
		std::function<void()> StartFunction = nullptr;
		std::function<void(float)> UpdateFunction = nullptr;
		std::function<void()> EndFunction = nullptr;
	};

	template<typename EnumType>
	void CreateState(EnumType _Key, std::function<void(float)> _UpdateFunction, std::function<void()> _Start = nullptr)
	{
		CreateState(static_cast<int>(_Key), _UpdateFunction, _Start);
	}

	void CreateState(int _Key, std::function<void(float)> _UpdateFunction, std::function<void()> _Start = nullptr)
	{
		if (true == States.contains(_Key))
		{
			MSGASSERT("이미 존재하는 스테이트를 또 만들려고 했습니다.");
			return;
		}

		States[_Key].UpdateFunction = _UpdateFunction;
		States[_Key].StartFunction = _Start;
	}

	template <typename EnumType>
	void ChangeStateFunction(EnumType _Key, std::function<void(float)> _UpdateFunction, std::function<void()> _Start = nullptr)
	{
		ChangeStateFunction(static_cast<int>(_Key), _UpdateFunction, _Start);
	}

	void ChangeStateFunction(int _Key, std::function<void(float)> _UpdateFunction, std::function<void()> _Start = nullptr)
	{
		if (States.contains(_Key) == false)
		{
			MSGASSERT("존재하지 않는 스테이트의 함수를 바꾸려고 했습니다.");
			return;
		}

		States[_Key].UpdateFunction = _UpdateFunction;
		States[_Key].StartFunction = _Start;
	}

	void Update(float _DeltaTime)
	{
		if (nullptr == CurStateFn)
		{
			MSGASSERT("상태가 지정되지 않은 스테이트머신 입니다.");
			return;
		}

		if (CurStateFn->UpdateFunction != nullptr)
		{
			CurStateFn->UpdateFunction(_DeltaTime);
		}
	}

	template<typename EnumType>
	void ChangeState(EnumType _Key)
	{
		ChangeState(static_cast<int>(_Key));
	}

	void ChangeState(int _Key)
	{
		if (false == States.contains(_Key))
		{
			MSGASSERT("만든적이 없는 스테이트로 체인지 하려고 했습니다.");
			return;
		}
		if (CurState == _Key)
		{
			return;
		}

		PreState = CurState;
		CurStateFn = &States[_Key];
		CurState = _Key;
		if (nullptr != CurStateFn->StartFunction)
		{
			CurStateFn->StartFunction();
		}
	}

	inline int GetState() const
	{
		return CurState;
	}
	inline int GetPrevState() const
	{
		return PreState;
	}

protected:

private:
	int PreState = -1;
	int CurState = -1;
	FSMFunction* CurStateFn = nullptr;
	std::map<int, FSMFunction> States;
};

