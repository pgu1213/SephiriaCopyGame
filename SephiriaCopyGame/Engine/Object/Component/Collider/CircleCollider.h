#pragma once
#include "Collider.h"

class CircleCollider : public Collider
{
public:
    CircleCollider(Object* owner, float radius);
public:
    bool CheckCollision(Collider* other) override;
    Rect GetBounds() override;
private:
    float radius;
};