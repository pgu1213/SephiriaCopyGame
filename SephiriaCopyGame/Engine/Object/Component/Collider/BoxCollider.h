#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
public:
    BoxCollider(Object* owner, const Vector2& size);
public:
    bool CheckCollision(Collider* other) override;
    Rect GetBounds() override;
private:
    Vector2 size;
};