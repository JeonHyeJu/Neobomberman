#pragma once
#include "SceneComponent.h"
#include "EngineSprite.h"
#include <EngineBase/EngineDelegate.h>
#include <map>
#include <EngineBase/EngineMath.h>

enum class PivotType
{
	Center,
	Bot,
	Top,
};

class USpriteRenderer : public USceneComponent
{
public:
	class FrameAnimation
	{
	public:
		UEngineSprite* Sprite = nullptr;
		std::vector<int> FrameIndex;
		std::vector<float> FrameTime;
		std::map<int, EngineDelegate> Events;

		int CurIndex = 0;
		int ResultIndex = 0;
		float CurTime = 0.0f;
		bool Loop = true;
		std::string Name = "";
		bool IsEnd = false;
		bool IsPaused = false;
		int Round = -1;
		int CurRound = 0;

		void Pause()
		{
			IsPaused = true;
		}
		void Resume()
		{
			IsPaused = false;
		}
		void Reset()
		{
			CurIndex = 0;
			CurTime = 0;
			ResultIndex = 0;
			IsEnd = false;
			IsPaused = false;
			CurRound = 0;
		}
	};

public:
	USpriteRenderer();
	~USpriteRenderer();

	USpriteRenderer(const USpriteRenderer& _Other) = delete;
	USpriteRenderer(USpriteRenderer&& _Other) noexcept = delete;
	USpriteRenderer& operator=(const USpriteRenderer& _Other) = delete;
	USpriteRenderer& operator=(USpriteRenderer&& _Other) noexcept = delete;

	void Render(float _DeltaTime);
	void BeginPlay() override;
	void ComponentTick(float _DeltaTime) override;

	template<typename EnumType>
	void SetOrder(EnumType _Order)
	{
		SetOrder(static_cast<int>(_Order));
	}

	void SetOrder(int _Order);

	int GetOrder()
	{
		return Order;
	}

	FVector2D SetSpriteScale(float _Ratio = 1.0f, int _CurIndex = 0);
	void CreateAnimation(std::string_view _AnimationName, std::string_view _SpriteName, int _Start, int _End, float Time = 0.1f, bool _Loop = true, int _Round = -1);
	void CreateAnimation(std::string_view _AnimationName, std::string_view _SpriteName, std::vector<int> _Indexs, std::vector<float> _Frame, bool _Loop = true, int _Round=-1);

	void CreateAnimation(std::string_view _AnimationName, std::string_view _SpriteName, std::vector<int> _Indexs, float _Frame, bool _Loop = true, int _Round = -1);

	void ChangeAnimation(std::string_view _AnimationName, bool _Force = false);
	void SetAnimationEvent(std::string_view _AnimationName, int _Frame, std::function<void()> _Function);

	std::string GetCurSpriteName()
	{
		return Sprite->GetName();
	}

	std::string GetCurAnimName()
	{
		std::string name = "";
		if (CurAnimation) {
			name = CurAnimation->Name;
		}
		return name;
	}

	void SetCameraEffect(bool _Value)
	{
		IsCameraEffect = _Value;
	}
	
	void SetPivot(FVector2D _Pivot)
	{
		Pivot = _Pivot;
	}

	void SetPivotType(PivotType _Type);

	void SetCameraEffectScale(float _Effect);
	void SetSprite(std::string_view _Name, int _CurIndex = 0);

	void ResumeCurAnimation()
	{
		if (CurAnimation)
		{
			CurAnimation->Resume();
		}
	}
	void PauseCurAnimation()
	{
		if (CurAnimation)
		{
			CurAnimation->Pause();
		}
	}
	bool IsCurAnimationPaused()
	{
		if (CurAnimation)
		{
			return CurAnimation->IsPaused;
		}
		else
		{
			return true;
		}
	}

	bool IsCurAnimationEnd()
	{
		if (CurAnimation)
		{
			return CurAnimation->IsEnd;
		}
		else
		{
			return true;
		}
	}

	void SetAlphaChar(unsigned char _Value)
	{
		Alpha = _Value;
	}

	void SetAnimationSpeed(float _Speed)
	{
		CurAnimationSpeed = _Speed;
	}

	void ResetAnimationSpeed()
	{
		CurAnimationSpeed = 1.0f;
	}

	void SetAlphafloat(float _Value)
	{
		_Value = UEngineMath::Clamp(_Value, 0.0f, 1.0f);
		Alpha = static_cast<unsigned char>(_Value * 255.0f);
	}

protected:

private:
	int Order = 0;
	int CurIndex = 0;
	bool IsCameraEffect = true;
	float CameraEffectScale = 1.0f;
	float CurAnimationSpeed = 1.0f;

	unsigned char Alpha = 255;

	FVector2D Pivot = FVector2D::ZERO;

	class UEngineSprite* Sprite = nullptr;

	std::map<std::string, FrameAnimation> FrameAnimations;
	FrameAnimation* CurAnimation = nullptr;
};

