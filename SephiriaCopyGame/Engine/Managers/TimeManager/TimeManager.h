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
    static bool Init(); // TimeManager �ʱ�ȭ �Լ�
    static void Update(); // �� ������ ȣ���Ͽ� ��Ÿ �ð��� ������Ʈ�ϴ� �Լ�
    static float GetDeltaTime(); // ���� �������� ��Ÿ �ð�(��)�� ��ȯ�ϴ� �Լ�
    static float GetTotalTime(); // ���α׷� ���� �� �� ��� �ð�(��)�� ��ȯ�ϴ� �Լ�
    static double GetTotalElapsedTimeDouble(); // ���е��� ���ؼ� double�� ����ؾ� �� ��

private:
    static LARGE_INTEGER m_PreviousCounter; // ���� �������� ī����
    static LARGE_INTEGER m_Frequency; // ���� ī������ ���ļ�
    static double m_CurrentDeltaTime; // ���� �������� ��Ÿ �ð�
    static double m_TotalElapsedTime; // �� ��� �ð�
};

