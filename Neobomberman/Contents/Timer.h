#pragma once
#include <thread>

class Timer
{
public:
	Timer() {}
	~Timer()
    {
        Stop();
        DeleteThread();
    }

	Timer(const Timer& _other) = delete;
	Timer(Timer&& _other) = delete;
	Timer& operator=(const Timer& _other) = delete;
	Timer& operator=(Timer&& _other) noexcept = delete;

    template <typename T, typename V>
    void SetTimeout(void (T::* _function)(), V* const _obj, int _delayMs)
    {
        Stop();
        DeleteThread();

        mIsStop = false;
        mThread = new std::thread(
            [=]()
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(_delayMs));
                if (mIsStop) return;
                (_obj->*_function)();
            }
        );
    }

    template <typename T, typename V>
    void SetInterval(void (T::* _function)(), V* const _obj, int _intervalMs)
    {
        Stop();
        DeleteThread();

        mIsStop = false;
        mThread = new std::thread(
            [=]()
            {
                while (true)
                {
                    std::chrono::system_clock::time_point nextTime = std::chrono::system_clock::now() + std::chrono::milliseconds(_intervalMs);

                    if (mIsStop) return;
                    (_obj->*_function)();

                    std::this_thread::sleep_until(nextTime);
                }
            }
        );
    }

    //template <typename T>
    //void SetTimeout(T _function, int _delayMs)
    //{
    //    Stop();
    //    DeleteThread();

    //    mIsStop = false;
    //    mThread = new std::thread(
    //        [=]()
    //        {
    //            std::this_thread::sleep_for(std::chrono::milliseconds(_delayMs));
    //            if (mIsStop) return;
    //            _function();
    //        }
    //    );
    //}

    //template <typename T>
    //void SetInterval(T _function, int _intervalMs)
    //{
    //    Stop();
    //    DeleteThread();

    //    mIsStop = false;
    //    mThread = new std::thread(
    //        [=]()
    //        {
    //            while (true)
    //            {
    //                std::chrono::system_clock::time_point nextTime = std::chrono::system_clock::now() + std::chrono::milliseconds(_intervalMs);

    //                if (mIsStop) return;
    //                _function();

    //                std::this_thread::sleep_until(nextTime);
    //            }
    //        }
    //    );
    //}

    void Stop()
    {
        mIsStop = true;
        Join();
    }

    void DeleteThread()
    {
        if (mThread)
        {
            delete mThread;
            mThread = nullptr;
        }
    }

    void Join()
    {
        if (mThread && mThread->joinable())
        {
            mThread->join();
        }
    }

    bool IsStopped() const
    {
        return mIsStop;
    }

private:
    bool mIsStop = true;
	std::thread* mThread = nullptr;
};