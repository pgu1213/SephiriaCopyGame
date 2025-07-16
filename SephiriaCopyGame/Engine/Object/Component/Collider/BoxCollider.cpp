#include <pch.h>
#include "BoxCollider.h"

BoxCollider::BoxCollider(Object* owner, const Vector2& size)
    : Collider(owner), size(size)
{

}

bool BoxCollider::CheckCollision(Collider* other)
{
    return false;
}

Rect BoxCollider::GetBounds()
{
    return Rect();
}
