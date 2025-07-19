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
    if (m_Owner && m_Owner->GetTransform())
    {
        return Vector2(
            m_Owner->GetTransform()->position.x + m_Offset.x,
            m_Owner->GetTransform()->position.y + m_Offset.y
        );
    }
    return m_Offset;
}

bool CircleCollider::CheckCollisionWithCircle(CircleCollider* other)
{
    Vector2 myCenter = GetCenter();
    Vector2 otherCenter = other->GetCenter();
    return CollisionUtils::CheckCircleCircle(myCenter, m_Radius, otherCenter, other->GetRadius());
}

bool CircleCollider::CheckCollisionWithBox(BoxCollider* other)
{
    Rect boxBounds = other->GetBounds();
    Vector2 myCenter = GetCenter();
    return CollisionUtils::CheckAABBCircle(boxBounds, myCenter, m_Radius);
}