#include "pch.h"
#include "DuelistMoleAI.h"
#include "Engine/Object/Object/Object.h"
#include "Engine/Managers/SceneManager/SceneManager.h"
#include "Engine/Scene/Scene.h"

DuelistMoleAI::DuelistMoleAI(Object* owner)
    : Component(owner)
    , m_CurrentState(DuelistMoleState::IDLE)
    , m_pTarget(nullptr)
    , m_DetectionRange(800.0f)
    , m_AttackRange(100.0f)
    , m_MoveSpeed(100.0f)
    , m_ChargeSpeed(200.0f)
    , m_PrepareTime(1.0f)
    , m_PrepareTimer(0.0f)
    , m_ChargeDirection(Vector2::Zero)
    , m_IsCharging(false)
{
}

DuelistMoleAI::DuelistMoleAI(const DuelistMoleAI& other)
    : Component(other.m_pObjOwner)
    , m_CurrentState(other.m_CurrentState)
    , m_pTarget(other.m_pTarget)
    , m_DetectionRange(other.m_DetectionRange)
    , m_AttackRange(other.m_AttackRange)
    , m_MoveSpeed(other.m_MoveSpeed)
    , m_ChargeSpeed(other.m_ChargeSpeed)
    , m_PrepareTime(other.m_PrepareTime)
    , m_PrepareTimer(other.m_PrepareTimer)
    , m_ChargeDirection(other.m_ChargeDirection)
    , m_IsCharging(other.m_IsCharging)
{
}

Component* DuelistMoleAI::CloneImpl() const
{
    return new DuelistMoleAI(*this);
}

void DuelistMoleAI::Update(float deltaTime)
{
    // 타겟이 없으면 플레이어를 자동으로 찾기
    if (!m_pTarget)
    {
        Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
        if (currentScene)
        {
            m_pTarget = currentScene->FindGameObject("Player");
        }

        if (!m_pTarget)
        {
            return; // 플레이어를 찾을 수 없으면 아무것도 하지 않음
        }
    }

    // 상태에 따른 업데이트
    switch (m_CurrentState)
    {
    case DuelistMoleState::IDLE:
        UpdateIdle(deltaTime);
        break;
    case DuelistMoleState::TRACKING:
        UpdateTracking(deltaTime);
        break;
    case DuelistMoleState::PREPARING:
        UpdatePreparing(deltaTime);
        break;
    case DuelistMoleState::CHARGING:
        UpdateCharging(deltaTime);
        break;
    }
}

void DuelistMoleAI::SetTarget(Object* target)
{
    m_pTarget = target;
}

void DuelistMoleAI::UpdateIdle(float deltaTime)
{
    // 인지 범위 안에 플레이어가 있는지 확인
    if (IsPlayerInRange(m_DetectionRange))
    {
        ChangeState(DuelistMoleState::TRACKING);
    }
}

void DuelistMoleAI::UpdateTracking(float deltaTime)
{
    // 플레이어가 인지 범위를 벗어났는지 확인
    if (!IsPlayerInRange(m_DetectionRange))
    {
        ChangeState(DuelistMoleState::IDLE);
        return;
    }

    // 플레이어가 공격 범위에 들어왔는지 확인
    if (IsPlayerInRange(m_AttackRange))
    {
        ChangeState(DuelistMoleState::PREPARING);
        return;
    }

    // 플레이어를 향해 이동
    MoveTowardsPlayer(deltaTime, m_MoveSpeed);
}

void DuelistMoleAI::UpdatePreparing(float deltaTime)
{
    // 준비 시간이 지났는지 확인
    m_PrepareTimer += deltaTime;

    if (m_PrepareTimer >= m_PrepareTime)
    {
        // 플레이어 방향으로 돌진 시작
        m_ChargeDirection = GetDirectionToPlayer();
        m_IsCharging = true;
        ChangeState(DuelistMoleState::CHARGING);
    }

    // 플레이어가 공격 범위를 벗어났다면 다시 추적
    if (!IsPlayerInRange(m_AttackRange))
    {
        ChangeState(DuelistMoleState::TRACKING);
    }
}

void DuelistMoleAI::UpdateCharging(float deltaTime)
{
    if (!m_IsCharging)
    {
        ChangeState(DuelistMoleState::IDLE);
        return;
    }

    // 돌진 방향으로 이동
    Vector2 currentPos = Vector2(
        static_cast<float>(m_pObjOwner->GetTransform().position.x),
        static_cast<float>(m_pObjOwner->GetTransform().position.y)
    );

    Vector2 velocity = m_ChargeDirection * m_ChargeSpeed * deltaTime;
    Vector2 newPos = currentPos + velocity;

    m_pObjOwner->SetPosition(newPos.x, newPos.y);

    // 돌진이 끝나는 조건들을 여기에 추가 가능
    // 예: 벽에 부딪혔을 때, 일정 거리를 이동했을 때, 일정 시간이 지났을 때 등
    // 지금은 간단하게 플레이어와의 거리가 일정 이상 벌어지면 돌진 종료
    if (GetDistanceToPlayer() > m_AttackRange * 1.5f)
    {
        m_IsCharging = false;
        ChangeState(DuelistMoleState::IDLE);
    }
}

bool DuelistMoleAI::IsPlayerInRange(float range)
{
    if (!m_pTarget)
        return false;

    return GetDistanceToPlayer() <= range;
}

float DuelistMoleAI::GetDistanceToPlayer()
{
    if (!m_pTarget)
        return FLT_MAX;

    Vector2 myPos = Vector2(
        static_cast<float>(m_pObjOwner->GetTransform().position.x),
        static_cast<float>(m_pObjOwner->GetTransform().position.y)
    );

    Vector2 playerPos = Vector2(
        static_cast<float>(m_pTarget->GetTransform().position.x),
        static_cast<float>(m_pTarget->GetTransform().position.y)
    );

    Vector2 diff = playerPos - myPos;

    return sqrtf(diff.x * diff.x + diff.y * diff.y);
}

Vector2 DuelistMoleAI::GetDirectionToPlayer()
{
    if (!m_pTarget)
        return Vector2::Zero;

    Vector2 myPos = Vector2(
        static_cast<float>(m_pObjOwner->GetTransform().position.x),
        static_cast<float>(m_pObjOwner->GetTransform().position.y)
    );

    Vector2 playerPos = Vector2(
        static_cast<float>(m_pTarget->GetTransform().position.x),
        static_cast<float>(m_pTarget->GetTransform().position.y)
    );

    Vector2 direction = playerPos - myPos;

    // Normalized 직접 계산
    float magnitude = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (magnitude > 0.0f)
    {
        return Vector2(direction.x / magnitude, direction.y / magnitude);
    }

    return Vector2::Zero;
}

void DuelistMoleAI::MoveTowardsPlayer(float deltaTime, float speed)
{
    if (!m_pTarget)
        return;

    Vector2 direction = GetDirectionToPlayer();
    Vector2 velocity = direction * speed * deltaTime;

    Vector2 currentPos = Vector2(
        static_cast<float>(m_pObjOwner->GetTransform().position.x),
        static_cast<float>(m_pObjOwner->GetTransform().position.y)
    );

    Vector2 newPos = currentPos + velocity;
    m_pObjOwner->SetPosition(newPos.x, newPos.y);
}

void DuelistMoleAI::ChangeState(DuelistMoleState newState)
{
    // 상태 변경 시 초기화 작업
    if (m_CurrentState != newState)
    {
        // 이전 상태 정리
        switch (m_CurrentState)
        {
        case DuelistMoleState::PREPARING:
            m_PrepareTimer = 0.0f;
            break;
        case DuelistMoleState::CHARGING:
            m_IsCharging = false;
            break;
        }

        // 새 상태 초기화
        switch (newState)
        {
        case DuelistMoleState::PREPARING:
            m_PrepareTimer = 0.0f;
            break;
        case DuelistMoleState::CHARGING:
            // 돌진 방향은 UpdatePreparing에서 설정됨
            break;
        }

        m_CurrentState = newState;
    }
}