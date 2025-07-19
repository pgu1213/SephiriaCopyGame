#include "pch.h"
#include "TimeManager.h"

LARGE_INTEGER TimeManager::m_PreviousCounter = { 0 };
LARGE_INTEGER TimeManager::m_Frequency = { 0 };
double TimeManager::m_CurrentDeltaTime = 0.0f;
double TimeManager::m_TotalElapsedTime = 0.0;

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

bool TimeManager::Init()
{
    // ���� ī������ ���ļ��� �����ͼ� ��� �������� üũ
    if (!QueryPerformanceFrequency(&m_Frequency))
    {
        return false;
    }

    // ù �������� ���� ī���� ��
    QueryPerformanceCounter(&m_PreviousCounter);

    // �ʱ�ȭ
    m_CurrentDeltaTime = 0.0f;
    m_TotalElapsedTime = 0.0;

	printf("TimeManager Initialized\n");

    return true;
}

// DeltaTime ������Ʈ
void TimeManager::Update()
{
    LARGE_INTEGER CurrentCounter; // ���� �������� ī���� ��
    QueryPerformanceCounter(&CurrentCounter); // ���� ī���� ��

    LONGLONG DeltaCounter = CurrentCounter.QuadPart - m_PreviousCounter.QuadPart; // ���� ī���� ���� ���� ī���� �� ����

    m_CurrentDeltaTime = ((double)DeltaCounter / m_Frequency.QuadPart); // ��Ÿ ī���� ���� ���ļ��� ������ ��Ÿ �ð� ��ȯ
    m_TotalElapsedTime += m_CurrentDeltaTime;     // �� ��� �ð��� ���� ��Ÿ �ð� �߰�
    m_PreviousCounter = CurrentCounter; // ���� �������� ����� ���� ���� ī���� ���� ���� ī���� ������ �����ϱ�
}

float TimeManager::GetDeltaTime()
{
    return static_cast<float>(m_CurrentDeltaTime);
}

float TimeManager::GetTotalTime()
{
    // Ȥ�ó� float �ʿ��ұ��
    return static_cast<float>(m_TotalElapsedTime);
}

double TimeManager::GetTotalElapsedTimeDouble()
{
    return m_TotalElapsedTime;
}
