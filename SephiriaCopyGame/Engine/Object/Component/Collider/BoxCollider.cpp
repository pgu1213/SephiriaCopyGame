#include <pch.h>
#include "BoxCollider.h"
#include "CircleCollider.h"

BoxCollider::BoxCollider(Object* owner)
	: Collider(owner)
    , m_Size(Vector2::Zero)
{
}

BoxCollider::BoxCollider(const BoxCollider& other)
	: Collider(other)
    , m_Size(other.m_Size)
{
}

Component* BoxCollider::CloneImpl() const
{
    return new BoxCollider(*this);
}

void BoxCollider::CopyFrom(const IPrototypeable* source)
{
    Component::CopyFrom(source);

    const BoxCollider* sourceCollider = dynamic_cast<const BoxCollider*>(source);
    if (sourceCollider)
    {
        m_Size = sourceCollider->m_Size;
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

void BoxCollider::Init()
{
    Collider::Init();
}

void BoxCollider::Update(float deltaTime)
{
    Collider::Update(deltaTime);
}

void BoxCollider::Render(HDC hdc)
{
    Collider::Render(hdc);
}

void BoxCollider::OnDestroy()
{
    Collider::OnDestroy();
}

bool BoxCollider::CheckCollision(Collider* other)
{
    if (BoxCollider* boxOther = dynamic_cast<BoxCollider*>(other))
    {
        return CheckCollisionWithBox(boxOther);
    }
    else if (CircleCollider* circleOther = dynamic_cast<CircleCollider*>(other))
    {
        return CheckCollisionWithCircle(circleOther);
    }
    return false;
}

Rect BoxCollider::GetBounds()
{
    Vector2 center = GetCenter();
    return Rect(
        static_cast<int>(center.x - m_Size.x / 2),
        static_cast<int>(center.y - m_Size.y / 2),
        static_cast<int>(m_Size.x),
        static_cast<int>(m_Size.y)
    );
}

Vector2 BoxCollider::GetCenter()
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

Vector2 BoxCollider::GetSize()
{
    return Vector2(m_Size.x * m_Scale.x, m_Size.y * m_Scale.y);
}

bool BoxCollider::CheckCollisionWithBox(BoxCollider* other)
{
    Rect myBounds = GetBounds();
    Rect otherBounds = other->GetBounds();

    return !(myBounds.X + myBounds.Width < otherBounds.X ||
        otherBounds.X + otherBounds.Width < myBounds.X ||
        myBounds.Y + myBounds.Height < otherBounds.Y ||
        otherBounds.Y + otherBounds.Height < myBounds.Y);
}

bool BoxCollider::CheckCollisionWithCircle(CircleCollider* other)
{
    Rect myBounds = GetBounds();
    Vector2 circleCenter = other->GetCenter();
    float radius = other->GetRadius();

    // 박스에서 원의 중심에 가장 가까운 점 찾기
    float closestX = max(static_cast<float>(myBounds.X),
        min(circleCenter.x, static_cast<float>(myBounds.X + myBounds.Width)));
    float closestY = max(static_cast<float>(myBounds.Y),
        min(circleCenter.y, static_cast<float>(myBounds.Y + myBounds.Height)));

    // 그 점과 원의 중심 사이의 거리 계산
    float dx = circleCenter.x - closestX;
    float dy = circleCenter.y - closestY;
    float distance = sqrt(dx * dx + dy * dy);

    return distance < radius;
}