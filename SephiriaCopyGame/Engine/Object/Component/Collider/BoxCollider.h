#pragma once
#include "Collider.h"

class CircleCollider;

class BoxCollider : public Collider
{
public:
    explicit BoxCollider(Object* owner);
    BoxCollider(const BoxCollider& other);
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

    void SetSize(const Vector2& size) { m_Size = size; }
    Vector2 GetSize();

    // 다른 콜라이더 타입과의 충돌 검사
    bool CheckCollisionWithBox(BoxCollider* other);
    bool CheckCollisionWithCircle(CircleCollider* other);

private:
    Vector2 m_Size;
	Object* m_Owner;
};