#pragma once
#include "Collider.h"

class BoxCollider;

class CircleCollider : public Collider
{
public:
    explicit CircleCollider(Object* owner);
    CircleCollider(const CircleCollider& other);
public:
    Component* CloneImpl() const override;
    void CopyFrom(const IPrototypeable* source) override;
    void Init() override;
    void Update(float deltaTime) override;
    void Render(HDC hdc) override;
    void OnDestroy() override;
public:
    bool CheckCollision(Collider* other) override;
    Rect GetBounds() override;
    Vector2 GetCenter() override;

    void SetRadius(float radius) { m_Radius = radius; }
    float GetRadius() { return m_Radius; }
	Vector2 GetSize() override { return Vector2(m_Radius * 2, m_Radius * 2); }
    // 다른 콜라이더 타입과의 충돌 검사
    bool CheckCollisionWithBox(BoxCollider* other);
    bool CheckCollisionWithCircle(CircleCollider* other);

private:
    float m_Radius;
    Object* m_Owner;
};