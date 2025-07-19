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