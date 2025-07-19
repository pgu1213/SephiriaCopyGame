#include <pch.h>
#include "CircleCollider.h"
#include "BoxCollider.h"

CircleCollider::CircleCollider(Object* owner, float radius)
    : Collider(owner), m_Radius(radius)
{
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