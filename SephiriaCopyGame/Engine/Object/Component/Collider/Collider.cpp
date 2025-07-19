#include <pch.h>
#include <functional>
#include "Collider.h"

Collider::Collider(Object* owner)
    : Component(owner)
    , m_Offset(0, 0)
    , m_IsTrigger(false)
    , m_Enabled(true)
    , m_Layer(CollisionLayer::None)
    , m_LayerMask(CollisionLayer::All)
{
    RegisterCollider(this);
}

void Collider::Update(float DeltaTime)
{
    if (!m_Enabled) return;

    // ���� �������� �浹 ������ ����
    m_PreviousCollisions = m_CurrentCollisions;
    m_CurrentCollisions.clear();

    // ��� �ٸ� �ݶ��̴��� �浹 �˻�
    CheckCollisionWithAll();

    // �̺�Ʈ ó��
    ProcessCollisionEvents();
}

void Collider::CheckCollisionWithAll()
{
    for (Collider* other : s_AllColliders)
    {
        if (other == this || !other->IsEnabled()) continue;
        if (!CanCollideWith(other->GetLayer())) continue;

        if (CheckCollision(other))
        {
            m_CurrentCollisions.insert(other);
        }
    }
}

void Collider::ProcessCollisionEvents()
{
    // Enter �̺�Ʈ (������ �����µ� ���� �ִ� ��)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) == m_PreviousCollisions.end())
        {
            // ���ο� �浹
            if (m_IsTrigger && m_OnTriggerEnter)
                m_OnTriggerEnter(collider);
            else if (!m_IsTrigger && m_OnCollisionEnter)
                m_OnCollisionEnter(collider);
        }
    }

    // Stay �̺�Ʈ (�������� �־��� ���絵 �ִ� ��)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) != m_PreviousCollisions.end())
        {
            // ���� ���� �浹
            if (m_IsTrigger && m_OnTriggerStay)
                m_OnTriggerStay(collider);
            else if (!m_IsTrigger && m_OnCollisionStay)
                m_OnCollisionStay(collider);
        }
    }

    // Exit �̺�Ʈ (������ �־��µ� ���� ���� ��)
    for (Collider* collider : m_PreviousCollisions)
    {
        if (m_CurrentCollisions.find(collider) == m_CurrentCollisions.end())
        {
            // �浹 ����
            if (m_IsTrigger && m_OnTriggerExit)
                m_OnTriggerExit(collider);
            else if (!m_IsTrigger && m_OnCollisionExit)
                m_OnCollisionExit(collider);
        }
    }
}

bool Collider::CanCollideWith(CollisionLayer otherLayer) const
{
    return static_cast<uint32_t>(m_LayerMask) & static_cast<uint32_t>(otherLayer);
}

void Collider::RegisterCollider(Collider* collider)
{
    s_AllColliders.push_back(collider);
}

void Collider::UnregisterCollider(Collider* collider)
{
    auto it = std::find(s_AllColliders.begin(), s_AllColliders.end(), collider);
    if (it != s_AllColliders.end())
    {
        s_AllColliders.erase(it);
    }
}