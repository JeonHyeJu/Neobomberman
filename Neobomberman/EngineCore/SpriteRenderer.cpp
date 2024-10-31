#include "PreCompile.h"
#include "SpriteRenderer.h"
#include "ImageManager.h"

#include "EngineAPICore.h"
#include <EngineBase/EngineString.h>

USpriteRenderer::USpriteRenderer()
{
}

USpriteRenderer::~USpriteRenderer()
{
}

void USpriteRenderer::Render(float _DeltaTime)
{
	if (nullptr != CurAnimation)
	{
		std::vector<int>& Indexs = CurAnimation->FrameIndex;
		std::vector<float>& Times = CurAnimation->FrameTime;

		Sprite = CurAnimation->Sprite;


		CurAnimation->CurTime += _DeltaTime;

		float CurFrameTime = Times[CurAnimation->CurIndex];

		//                           0.1 0.1 0.1
		if (CurAnimation->CurTime > CurFrameTime)
		{
			CurAnimation->CurTime -= CurFrameTime;
			++CurAnimation->CurIndex;

			if (CurAnimation->CurIndex >= Indexs.size())
			{
				if (true == CurAnimation->Loop)
				{
					CurAnimation->CurIndex = 0;
				}
				else
				{
					--CurAnimation->CurIndex;
				}
			}
		}

		//         2 3 4           0
		CurIndex = Indexs[CurAnimation->CurIndex];
		// ++CurAnimation->CurIndex;
	}

	if (nullptr == Sprite)
	{
		MSGASSERT("스프라이트가 세팅되지 않은 액터를 랜더링을 할수 없습니다.");
		return;
	}

	UEngineWindow& MainWindow = UEngineAPICore::GetCore()->GetMainWindow();
	UEngineWinImage* BackBufferImage = MainWindow.GetBackBuffer();
	UEngineSprite::USpriteData CurData = Sprite->GetSpriteData(CurIndex);
	CurData.Image->CopyToTrans(BackBufferImage, GetActorTransform(), CurData.Transform);
}

void USpriteRenderer::BeginPlay()
{
	Super::BeginPlay();

	AActor* Actor = GetActor();
	ULevel* Level = Actor->GetWorld();

	Level->PushRenderer(this);
}

void USpriteRenderer::ComponentTick(float _DeltaTime)
{
	Super::ComponentTick(_DeltaTime);
}

void USpriteRenderer::SetSprite(std::string_view _Name, int _CurIndex /*= 0*/)
{
	Sprite = UImageManager::GetInst().FindSprite(_Name);

	if (nullptr == Sprite)
	{
		MSGASSERT("로드하지 않은 스프라이트를 사용하려고 했습니다" + std::string(_Name));
		return;
	}

	CurIndex = _CurIndex;
}

void USpriteRenderer::SetOrder(int _Order)
{
	int PrevOrder = Order;

	Order = _Order;

	ULevel* Level = GetActor()->GetWorld();

	if (nullptr != Level)
	{
		Level->ChangeRenderOrder(this, PrevOrder);
	}
}

FVector2D USpriteRenderer::SetSpriteScale(float _Ratio /*= 1.0f*/, int _CurIndex /*= 0*/)
{
	if (nullptr == Sprite)
	{
		MSGASSERT("스프라이트를 세팅하지 않고 스프라이트 크기로 랜더러 크기를 조정할수 없습니다.");
		return FVector2D::ZERO;
	}

	UEngineSprite::USpriteData CurData = Sprite->GetSpriteData(CurIndex);

	FVector2D Scale = CurData.Transform.Scale* _Ratio;

	SetComponentScale(CurData.Transform.Scale * _Ratio);

	return Scale;
}


void USpriteRenderer::CreateAnimation(std::string_view _AnimationName, std::string_view _SpriteName, int _Start, int _End, float Time /*= 0.1f*/, bool _Loop /*= true*/)
{
	if (_Start > _End)
	{
		MSGASSERT("애니메이션에서 Start가 End보다 클수는 없습니다. " + std::string(_AnimationName));
		return;
	}

	int Inter = (_End - _Start) + 1;

	std::vector<int> Indexs;
	std::vector<float> Times;

	for (size_t i = 0; i < Inter; i++)
	{
		Indexs.push_back(_Start);
		Times.push_back(Time);
		++_Start;
	}

	CreateAnimation(_AnimationName, _SpriteName, Indexs, Times, _Loop);
}

void USpriteRenderer::CreateAnimation(std::string_view _AnimationName, std::string_view _SpriteName, std::vector<int> _Indexs, std::vector<float> _Frame, bool _Loop /*= true*/)
{
	std::string UpperName = UEngineString::ToUpper(_AnimationName);

	if (_Frame.size() != _Indexs.size())
	{
		MSGASSERT(UpperName + "을 만들다 에러가 났습니다 프레임과 타임의 카운트가 서로 다릅니다");
		return;
	}

	if (FrameAnimations.contains(UpperName))
	{
		return;
	}

	UEngineSprite* FindSprite = UImageManager::GetInst().FindSprite(_SpriteName);

	if (nullptr == FindSprite)
	{
		MSGASSERT("로드하지 않은 스프라이트를 애니메이션 생성에 사용하려고 했습니다" + std::string(UpperName));
		return;
	}

	FrameAnimation NewAnimation;
	NewAnimation.Sprite = FindSprite;
	NewAnimation.FrameIndex = _Indexs;
	NewAnimation.FrameTime = _Frame;
	NewAnimation.Loop = _Loop;
	NewAnimation.Reset();

	FrameAnimations.insert({ UpperName, NewAnimation });
}

void USpriteRenderer::ChangeAnimation(std::string_view _AnimationName, bool _Force /*= false*/)
{
	std::string UpperName = UEngineString::ToUpper(_AnimationName);

	if (false == FrameAnimations.contains(UpperName))
	{
		MSGASSERT("존재하지 않은 애니메이션으로 변경하려고 했습니다. = " + UpperName);
		return;
	}

	FrameAnimation* ChangeAnimation =&FrameAnimations[UpperName];

	if (CurAnimation == ChangeAnimation && false == _Force)
	{
		return;
	}

	CurAnimation = &FrameAnimations[UpperName];
	CurAnimation->Reset();
}