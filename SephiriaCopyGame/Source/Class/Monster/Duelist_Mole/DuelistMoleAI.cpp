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
    // Ÿ���� ������ �÷��̾ �ڵ����� ã��
    if (!m_pTarget)
    {
        Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
        if (currentScene)
        {
            m_pTarget = currentScene->FindGameObject("Player");
        }

        if (!m_pTarget)
        {
            return; // �÷��̾ ã�� �� ������ �ƹ��͵� ���� ����
        }
    }

    // ���¿� ���� ������Ʈ
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
    // ���� ���� �ȿ� �÷��̾ �ִ��� Ȯ��
    if (IsPlayerInRange(m_DetectionRange))
    {
        ChangeState(DuelistMoleState::TRACKING);
    }
}

void DuelistMoleAI::UpdateTracking(float deltaTime)
{
    // �÷��̾ ���� ������ ������� Ȯ��
    if (!IsPlayerInRange(m_DetectionRange))
    {
        ChangeState(DuelistMoleState::IDLE);
        return;
    }

    // �÷��̾ ���� ������ ���Դ��� Ȯ��
    if (IsPlayerInRange(m_AttackRange))
    {
        ChangeState(DuelistMoleState::PREPARING);
        return;
    }

    // �÷��̾ ���� �̵�
    MoveTowardsPlayer(deltaTime, m_MoveSpeed);
}

void DuelistMoleAI::UpdatePreparing(float deltaTime)
{
    // �غ� �ð��� �������� Ȯ��
    m_PrepareTimer += deltaTime;

    if (m_PrepareTimer >= m_PrepareTime)
    {
        // �÷��̾� �������� ���� ����
        m_ChargeDirection = GetDirectionToPlayer();
        m_IsCharging = true;
        ChangeState(DuelistMoleState::CHARGING);
    }

    // �÷��̾ ���� ������ ����ٸ� �ٽ� ����
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

    // ���� �������� �̵�
    Vector2 currentPos = Vector2(
        static_cast<float>(m_pObjOwner->GetTransform().position.x),
        static_cast<float>(m_pObjOwner->GetTransform().position.y)
    );

    Vector2 velocity = m_ChargeDirection * m_ChargeSpeed * deltaTime;
    Vector2 newPos = currentPos + velocity;

    m_pObjOwner->SetPosition(newPos.x, newPos.y);

    // ������ ������ ���ǵ��� ���⿡ �߰� ����
    // ��: ���� �ε����� ��, ���� �Ÿ��� �̵����� ��, ���� �ð��� ������ �� ��
    // ������ �����ϰ� �÷��̾���� �Ÿ��� ���� �̻� �������� ���� ����
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

    // Normalized ���� ���
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
    // ���� ���� �� �ʱ�ȭ �۾�
    if (m_CurrentState != newState)
    {
        // ���� ���� ����
        switch (m_CurrentState)
        {
        case DuelistMoleState::PREPARING:
            m_PrepareTimer = 0.0f;
            break;
        case DuelistMoleState::CHARGING:
            m_IsCharging = false;
            break;
        }

        // �� ���� �ʱ�ȭ
        switch (newState)
        {
        case DuelistMoleState::PREPARING:
            m_PrepareTimer = 0.0f;
            break;
        case DuelistMoleState::CHARGING:
            // ���� ������ UpdatePreparing���� ������
            break;
        }

        m_CurrentState = newState;
    }
}