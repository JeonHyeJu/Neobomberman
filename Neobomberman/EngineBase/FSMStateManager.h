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
			MSGASSERT("�̹� �����ϴ� ������Ʈ�� �� ������� �߽��ϴ�.");
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
			MSGASSERT("�������� �ʴ� ������Ʈ�� �Լ��� �ٲٷ��� �߽��ϴ�.");
			return;
		}

		States[_Key].UpdateFunction = _UpdateFunction;
		States[_Key].StartFunction = _Start;
	}

	void Update(float _DeltaTime)
	{
		if (nullptr == CurStateFn)
		{
			MSGASSERT("���°� �������� ���� ������Ʈ�ӽ� �Դϴ�.");
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
			MSGASSERT("�������� ���� ������Ʈ�� ü���� �Ϸ��� �߽��ϴ�.");
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

