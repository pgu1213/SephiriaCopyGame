#pragma once
#include <Engine/Managers/SingletonManager/SingletonManager.h>

class TimeManager : public SingleTon<TimeManager>
{
    friend class SingleTon<TimeManager>;
private:
    explicit TimeManager();
public:
    virtual ~TimeManager();
public:
    static bool Init(); // TimeManager 초기화 함수
    static void Update(); // 매 프레임 호출하여 델타 시간을 업데이트하는 함수
    static float GetDeltaTime(); // 현재 프레임의 델타 시간(초)을 반환하는 함수
    static float GetTotalTime(); // 프로그램 시작 후 총 경과 시간(초)을 반환하는 함수
    static double GetTotalElapsedTimeDouble(); // 정밀도를 위해서 double을 사용해야 할 때

private:
    static LARGE_INTEGER m_PreviousCounter; // 이전 프레임의 카운터
    static LARGE_INTEGER m_Frequency; // 고성능 카운터의 주파수
    static double m_CurrentDeltaTime; // 현재 프레임의 델타 시간
    static double m_TotalElapsedTime; // 총 경과 시간
};

