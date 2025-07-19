#pragma once
#include "Collider.h"

class BoxCollider;

class CircleCollider : public Collider
{
public:
    CircleCollider(Object* owner, float radius);

public:
    bool CheckCollision(Collider* other) override;
    Rect GetBounds() override;
    Vector2 GetCenter() override;

    void SetRadius(float radius) { m_Radius = radius; }
    float GetRadius() const { return m_Radius; }

    // �ٸ� �ݶ��̴� Ÿ�԰��� �浹 �˻�
    bool CheckCollisionWithBox(BoxCollider* other);
    bool CheckCollisionWithCircle(CircleCollider* other);

private:
    float m_Radius;
    Object* m_Owner;
};