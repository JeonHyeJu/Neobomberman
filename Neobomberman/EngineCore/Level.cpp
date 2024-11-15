#include "PreCompile.h"
#include "Level.h"
#include "EngineAPICore.h"

#include <EngineBase/EngineMath.h>
#include <EnginePlatform/EngineWindow.h>
#include <EnginePlatform/EngineWinImage.h>

#include "SpriteRenderer.h"
#include "2DCollision.h"

#include "EngineCoreDebug.h"

ULevel::ULevel()
{
}

ULevel::~ULevel()
{
	{
		std::list<AActor*>::iterator StartIter = BeginPlayList.begin();
		std::list<AActor*>::iterator EndIter = BeginPlayList.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			AActor* CurActor = *StartIter;
			if (nullptr != CurActor)
			{
				delete CurActor;
			}
		}
	}
	

	{
		std::list<AActor*>::iterator StartIter = AllActors.begin();
		std::list<AActor*>::iterator EndIter = AllActors.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			AActor* CurActor = *StartIter;
			if (nullptr != CurActor)
			{
				delete CurActor;
			}
		}
	}
}

void ULevel::BeginPlayCheck()
{
	{
		std::list<AActor*>::iterator StartIter = BeginPlayList.begin();
		std::list<AActor*>::iterator EndIter = BeginPlayList.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			AActor* CurActor = *StartIter;
			CurActor->BeginPlay();
			AllActors.push_back(CurActor);
		}

		BeginPlayList.clear();
		AActor::ComponentBeginPlay();
	}


}

void ULevel::LevelChangeStart()
{
	BeginPlayCheck();

	{
		{
			std::list<AActor*>::iterator StartIter = AllActors.begin();
			std::list<AActor*>::iterator EndIter = AllActors.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				AActor* CurActor = *StartIter;

				CurActor->LevelChangeStart();
			}
		}

		{
			std::list<AActor*>::iterator StartIter = BeginPlayList.begin();
			std::list<AActor*>::iterator EndIter = BeginPlayList.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				AActor* CurActor = *StartIter;

				CurActor->LevelChangeStart();
			}
		}
	}

}

void ULevel::LevelChangeEnd()
{
	{
		{
			std::list<AActor*>::iterator StartIter = AllActors.begin();
			std::list<AActor*>::iterator EndIter = AllActors.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				AActor* CurActor = *StartIter;

				CurActor->LevelChangeEnd();
			}
		}

		{
			std::list<AActor*>::iterator StartIter = BeginPlayList.begin();
			std::list<AActor*>::iterator EndIter = BeginPlayList.end();

			for (; StartIter != EndIter; ++StartIter)
			{
				AActor* CurActor = *StartIter;

				CurActor->LevelChangeEnd();
			}
		}
	}

}

void ULevel::Tick(float _DeltaTime)
{
	BeginPlayCheck();

	{
		std::list<AActor*>::iterator StartIter = AllActors.begin();
		std::list<AActor*>::iterator EndIter = AllActors.end();

		for (; StartIter != EndIter; ++StartIter)
		{
			AActor* CurActor = *StartIter;

			if (false == CurActor->IsActive())
			{
				continue;
			}

			CurActor->Tick(_DeltaTime);
		}
	}
}

void ULevel::Render(float _DeltaTime)
{
	ScreenClear();

	if (true == IsCameraToMainPawn)
	{
		// CameraPivot = FVector2D(-1280, -720) * 0.5f;
		CameraPos = MainPawn->GetTransform().Location + CameraPivot;
	}

	std::map<int, std::list<class USpriteRenderer*>>::iterator StartOrderIter = Renderers.begin();
	std::map<int, std::list<class USpriteRenderer*>>::iterator EndOrderIter = Renderers.end();

	for (; StartOrderIter != EndOrderIter; ++StartOrderIter)
	{
		std::list<class USpriteRenderer*>& RendererList = StartOrderIter->second;

		std::list<class USpriteRenderer*>::iterator RenderStartIter = RendererList.begin();
		std::list<class USpriteRenderer*>::iterator RenderEndIter = RendererList.end();

		for (; RenderStartIter != RenderEndIter; ++RenderStartIter)
		{
			if (false == (*RenderStartIter)->IsActive())
			{
				continue;
			}

			(*RenderStartIter)->Render(_DeltaTime);
		}
	}

	UEngineDebug::PrintEngineDebugRender();

	DoubleBuffering();
}

void ULevel::Collision(float _DeltaTime)
{
	for (size_t i = 0; i < CollisionLink.size(); i++)
	{
		CollisionLinkData Data = CollisionLink[i];

		int Left = Data.Left;
		int Right = Data.Right;
		
		std::list<class U2DCollision*>& LeftList = CheckCollisions[Left];

		std::list<class U2DCollision*>& RightList = Collisions[Right];

		std::list<class U2DCollision*>::iterator StartLeftIter = LeftList.begin();
		std::list<class U2DCollision*>::iterator EndLeftIter = LeftList.end();

		std::list<class U2DCollision*>::iterator EndRightIter = RightList.end();

		for (; StartLeftIter != EndLeftIter; ++StartLeftIter)
		{
		 	U2DCollision* LeftCollision = *StartLeftIter;

			if (false == LeftCollision->IsActive())
			{
				continue;
			}

			std::list<class U2DCollision*>::iterator StartRightIter = RightList.begin();

			for (; StartRightIter != EndRightIter; ++StartRightIter)
			{
				U2DCollision* RightCollision = *StartRightIter;
				if (LeftCollision == RightCollision)
				{
					continue;
				}

				if (false == RightCollision->IsActive())
				{
					continue;
				}

				LeftCollision->CollisionEventCheck(RightCollision);
			}
		}
	}
}

// 엔진 이벤트코드니까 이상한 곳에서 할필요가 없다.
// 컨텐츠에서는 존재하는지도 몰라야 한다.


void ULevel::Release(float _DeltaTime)
{
	std::list<AActor*>::iterator StartIter = AllActors.begin();
	std::list<AActor*>::iterator EndIter = AllActors.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		AActor* CurActor = *StartIter;
		CurActor->ReleaseTimeCheck(_DeltaTime);
	}

	{
		std::map<int, std::list<class U2DCollision*>>::iterator StartOrderIter = Collisions.begin();
		std::map<int, std::list<class U2DCollision*>>::iterator EndOrderIter = Collisions.end();

		for (; StartOrderIter != EndOrderIter; ++StartOrderIter)
		{
			std::list<class U2DCollision*>& CollisionList = StartOrderIter->second;

			std::list<class U2DCollision*>::iterator CollisionStartIter = CollisionList.begin();
			std::list<class U2DCollision*>::iterator CollisionEndIter = CollisionList.end();
			
			for (; CollisionStartIter != CollisionEndIter; )
			{
				if (false == (*CollisionStartIter)->IsDestroy())
				{
					++CollisionStartIter;
					continue;
				}

				CollisionStartIter = CollisionList.erase(CollisionStartIter);
			}
		}
	}

	{
		std::map<int, std::list<class U2DCollision*>>::iterator StartOrderIter = CheckCollisions.begin();
		std::map<int, std::list<class U2DCollision*>>::iterator EndOrderIter = CheckCollisions.end();

		for (; StartOrderIter != EndOrderIter; ++StartOrderIter)
		{
			std::list<class U2DCollision*>& CollisionList = StartOrderIter->second;

			std::list<class U2DCollision*>::iterator CollisionStartIter = CollisionList.begin();
			std::list<class U2DCollision*>::iterator CollisionEndIter = CollisionList.end();

			for (; CollisionStartIter != CollisionEndIter; )
			{
				if (false == (*CollisionStartIter)->IsDestroy())
				{
					++CollisionStartIter;
					continue;
				}

				CollisionStartIter = CollisionList.erase(CollisionStartIter);
			}
		}
	}

	{
		std::map<int, std::list<class USpriteRenderer*>>::iterator StartOrderIter = Renderers.begin();
		std::map<int, std::list<class USpriteRenderer*>>::iterator EndOrderIter = Renderers.end();

	for (; StartOrderIter != EndOrderIter; ++StartOrderIter)
	{
		std::list<class USpriteRenderer*>& RendererList = StartOrderIter->second;

		std::list<class USpriteRenderer*>::iterator RenderStartIter = RendererList.begin();
		std::list<class USpriteRenderer*>::iterator RenderEndIter = RendererList.end();

		for (; RenderStartIter != RenderEndIter; )
		{
			if (false == (*RenderStartIter)->IsDestroy())
			{
				++RenderStartIter;
				continue;
			}

				RenderStartIter = RendererList.erase(RenderStartIter);
			}
		}
	}

	{
		std::list<AActor*>::iterator StartIter = AllActors.begin();
		std::list<AActor*>::iterator EndIter = AllActors.end();

		for (; StartIter != EndIter; )
		{
			AActor* CurActor = *StartIter;


			if (false == CurActor->IsDestroy())
			{
				CurActor->ReleaseCheck(_DeltaTime);
				++StartIter;
				continue;
			}

			delete CurActor;
			StartIter = AllActors.erase(StartIter);
		}
	}
}

void ULevel::ScreenClear()
{
	UEngineWindow& MainWindow = UEngineAPICore::GetCore()->GetMainWindow();
	UEngineWinImage* BackBufferImage = MainWindow.GetBackBuffer();
	FVector2D Size = MainWindow.GetWindowSize();

	Rectangle(BackBufferImage->GetDC(), -1, -1, Size.iX() + 2, Size.iY() + 2);
}

void ULevel::DoubleBuffering()
{
	UEngineWindow& MainWindow = UEngineAPICore::GetCore()->GetMainWindow();

	UEngineWinImage* WindowImage = MainWindow.GetWindowImage();
	UEngineWinImage* BackBufferImage = MainWindow.GetBackBuffer();

	FTransform Trans;
	Trans.Location = MainWindow.GetWindowSize().Half();
	Trans.Scale = MainWindow.GetWindowSize();

	BackBufferImage->CopyToBit(WindowImage, Trans);
}

void ULevel::PushRenderer(class USpriteRenderer* _Renderer)
{
	int Order = _Renderer->GetOrder();

	Renderers[Order].push_back(_Renderer);
}

void ULevel::PushCollision(U2DCollision* _Collision)
{
	int Order = _Collision->GetGroup();
	Collisions[Order].push_back(_Collision);
}

std::vector<CollisionLinkData> ULevel::CollisionLink;

void ULevel::PushCheckCollision(class U2DCollision* _Collision)
{
	int Order = _Collision->GetGroup();
	CheckCollisions[Order].push_back(_Collision);
}

void ULevel::ChangeRenderOrder(class USpriteRenderer* _Renderer, int _PrevOrder)
{
	Renderers[_PrevOrder].remove(_Renderer);
	Renderers[_Renderer->GetOrder()].push_back(_Renderer);
}