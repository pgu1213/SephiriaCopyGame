#include <pch.h>
#include "BoxCollider.h"
#include "CircleCollider.h"

BoxCollider::BoxCollider(Object* owner, const Vector2& size)
    : Collider(owner), m_Size(size)
{
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
    if (m_Owner && m_Owner->GetTransform())
    {
        return Vector2(
            m_Owner->GetTransform()->position.x + m_Offset.x,
            m_Owner->GetTransform()->position.y + m_Offset.y
        );
    }
    return m_Offset;
}

bool BoxCollider::CheckCollisionWithBox(BoxCollider* other)
{
    Rect myBounds = GetBounds();
    Rect otherBounds = other->GetBounds();
    return CollisionUtils::CheckAABB(myBounds, otherBounds);
}

bool BoxCollider::CheckCollisionWithCircle(CircleCollider* other)
{
    Rect myBounds = GetBounds();
    Vector2 circleCenter = other->GetCenter();
    float radius = other->GetRadius();
    return CollisionUtils::CheckAABBCircle(myBounds, circleCenter, radius);
}