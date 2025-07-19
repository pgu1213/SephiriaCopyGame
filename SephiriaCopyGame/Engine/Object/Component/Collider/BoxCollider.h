#pragma once
#include "Collider.h"

class CircleCollider;

class BoxCollider : public Collider
{
public:
    BoxCollider(Object* owner, const Vector2& size);

public:
    bool CheckCollision(Collider* other) override;
    Rect GetBounds() override;
    Vector2 GetCenter() override;

    void SetSize(const Vector2& size) { m_Size = size; }
    Vector2 GetSize() const { return m_Size; }

    // �ٸ� �ݶ��̴� Ÿ�԰��� �浹 �˻�
    bool CheckCollisionWithBox(BoxCollider* other);
    bool CheckCollisionWithCircle(CircleCollider* other);

private:
    Vector2 m_Size;
	Object* m_Owner;
};