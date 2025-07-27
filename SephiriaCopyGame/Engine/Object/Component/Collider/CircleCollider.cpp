#include <pch.h>
#include "CircleCollider.h"
#include "BoxCollider.h"

CircleCollider::CircleCollider(Object* owner)
    : Collider(owner)
    , m_Radius(0.f)
{
}

CircleCollider::CircleCollider(const CircleCollider& other)
    : Collider(other)
    , m_Radius(other.m_Radius)
{
}

Component* CircleCollider::CloneImpl() const
{
    return new CircleCollider(*this);
}

void CircleCollider::CopyFrom(const IPrototypeable* source)
{
    Component::CopyFrom(source);
    const CircleCollider* sourceCollider = dynamic_cast<const CircleCollider*>(source);
    if (sourceCollider)
    {
        m_Radius = sourceCollider->m_Radius;
        m_Offset = sourceCollider->m_Offset;
        m_IsTrigger = sourceCollider->m_IsTrigger;
        m_Layer = sourceCollider->m_Layer;
        m_LayerMask = sourceCollider->m_LayerMask;
        m_Enabled = sourceCollider->m_Enabled;
        // 이벤트 콜백 복사
        m_OnCollisionEnter = sourceCollider->m_OnCollisionEnter;
        m_OnCollisionStay = sourceCollider->m_OnCollisionStay;
        m_OnCollisionExit = sourceCollider->m_OnCollisionExit;
        m_OnTriggerEnter = sourceCollider->m_OnTriggerEnter;
        m_OnTriggerStay = sourceCollider->m_OnTriggerStay;
        m_OnTriggerExit = sourceCollider->m_OnTriggerExit;
	}
}

void CircleCollider::Init()
{
    Collider::Init();
}

void CircleCollider::Update(float deltaTime)
{
    Collider::Update(deltaTime);
}

void CircleCollider::Render(HDC hdc)
{
    Collider::Render(hdc);
}

void CircleCollider::OnDestroy()
{
    Collider::OnDestroy();
}

bool CircleCollider::CheckCollision(Collider* other)
{
    if (CircleCollider* circleOther = dynamic_cast<CircleCollider*>(other))
    {
        return CheckCollisionWithCircle(circleOther);
    }
    else if (BoxCollider* boxOther = dynamic_cast<BoxCollider*>(other))
    {
        return CheckCollisionWithBox(boxOther);
    }
    return false;
}

Rect CircleCollider::GetBounds()
{
    Vector2 center = GetCenter();
    return Rect(
        static_cast<int>(center.x - m_Radius),
        static_cast<int>(center.y - m_Radius),
        static_cast<int>(m_Radius * 2),
        static_cast<int>(m_Radius * 2)
    );
}

Vector2 CircleCollider::GetCenter()
{
    if (m_Owner)
    {
        Transform transform = m_Owner->GetTransform();
        
        return Vector2(
            transform.position.x + m_Offset.x,
            transform.position.y + m_Offset.y
        );
    }
    return m_Offset;
}

bool CircleCollider::CheckCollisionWithCircle(CircleCollider* other)
{
    Vector2 myCenter = GetCenter();
    Vector2 otherCenter = other->GetCenter();

    float dx = myCenter.x - otherCenter.x;
    float dy = myCenter.y - otherCenter.y;
    float distance = sqrt(dx * dx + dy * dy);

    return distance < (m_Radius + other->GetRadius());
}

bool CircleCollider::CheckCollisionWithBox(BoxCollider* other)
{
    // BoxCollider의 함수를 재사용
    return other->CheckCollisionWithCircle(this);
}