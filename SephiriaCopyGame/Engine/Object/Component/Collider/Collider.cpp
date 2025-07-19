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

    // 이전 프레임의 충돌 정보를 저장
    m_PreviousCollisions = m_CurrentCollisions;
    m_CurrentCollisions.clear();

    // 모든 다른 콜라이더와 충돌 검사
    CheckCollisionWithAll();

    // 이벤트 처리
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
    // Enter 이벤트 (이전에 없었는데 현재 있는 것)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) == m_PreviousCollisions.end())
        {
            // 새로운 충돌
            if (m_IsTrigger && m_OnTriggerEnter)
                m_OnTriggerEnter(collider);
            else if (!m_IsTrigger && m_OnCollisionEnter)
                m_OnCollisionEnter(collider);
        }
    }

    // Stay 이벤트 (이전에도 있었고 현재도 있는 것)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) != m_PreviousCollisions.end())
        {
            // 지속 중인 충돌
            if (m_IsTrigger && m_OnTriggerStay)
                m_OnTriggerStay(collider);
            else if (!m_IsTrigger && m_OnCollisionStay)
                m_OnCollisionStay(collider);
        }
    }

    // Exit 이벤트 (이전에 있었는데 현재 없는 것)
    for (Collider* collider : m_PreviousCollisions)
    {
        if (m_CurrentCollisions.find(collider) == m_CurrentCollisions.end())
        {
            // 충돌 끝남
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