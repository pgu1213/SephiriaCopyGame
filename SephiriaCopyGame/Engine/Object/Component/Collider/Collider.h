#pragma once
#include <Engine/Object/Component/Component.h>

class Collider : public Component
{
public:
    Collider(Object* owner);

    virtual bool CheckCollision(Collider* other) = 0;
    virtual Rect GetBounds() = 0;

    // 충돌 이벤트
    function<void(Collider*)> OnCollisionEnter;
    function<void(Collider*)> OnCollisionExit;
    function<void(Collider*)> OnTriggerEnter;
    function<void(Collider*)> OnTriggerExit;
protected:
    Vector2 offset;
    bool isTrigger;
};