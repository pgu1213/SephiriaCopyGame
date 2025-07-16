#include <pch.h>
#include "CircleCollider.h"

CircleCollider::CircleCollider(Object* owner, float radius)
    : Collider(owner), radius(radius) 
{

}

bool CircleCollider::CheckCollision(Collider* other)
{
    return false;
}

Rect CircleCollider::GetBounds()
{
    return Rect();
}
