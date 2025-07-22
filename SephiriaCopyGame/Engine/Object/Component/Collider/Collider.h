#pragma once
#include <Engine/Object/Component/Component.h>

class Collider;

using CollisionCallback = function<void(Collider*)>;

class Collider : public Component
{
public:
    Collider(Object* owner);
    virtual ~Collider();

public:
    // 순수 가상 함수들
    virtual bool CheckCollision(Collider* other) = 0;
    virtual Rect GetBounds() = 0;
    virtual Vector2 GetCenter() = 0;

    // 충돌 검사 모든 콜라이더와 체크
	virtual void Update(float DeltaTime) override;
	virtual void Render(HDC hdc) override;

    // 콜백 함수
    void SetOnCollisionEnter(const CollisionCallback& callback) { m_OnCollisionEnter = callback; }
    void SetOnCollisionStay(const CollisionCallback& callback) { m_OnCollisionStay = callback; }
    void SetOnCollisionExit(const CollisionCallback& callback) { m_OnCollisionExit = callback; }

    void SetOnTriggerEnter(const CollisionCallback& callback) { m_OnTriggerEnter = callback; }
    void SetOnTriggerStay(const CollisionCallback& callback) { m_OnTriggerStay = callback; }
    void SetOnTriggerExit(const CollisionCallback& callback) { m_OnTriggerExit = callback; }

    // 속성
    void SetIsTrigger(bool trigger) { m_IsTrigger = trigger; }
    void SetLayer(CollisionLayer layer) { m_Layer = layer; }
    void SetLayerMask(CollisionLayer mask) { m_LayerMask = mask; }
    void SetEnabled(bool enabled) { m_Enabled = enabled; }

    bool IsTrigger() const { return m_IsTrigger; }
    CollisionLayer GetLayer() const { return m_Layer; }
    bool IsEnabled() const { return m_Enabled; }

    // 레이어 마스크 체크
    bool CanCollideWith(CollisionLayer otherLayer) const;

protected:
    Vector2 m_Offset;
    bool m_IsTrigger;
    bool m_Enabled;
    CollisionLayer m_Layer;
    CollisionLayer m_LayerMask;

    // 이전 프레임 충돌 상태 추적
    set<Collider*> m_PreviousCollisions;
    set<Collider*> m_CurrentCollisions;

    // 이벤트 콜백들
    CollisionCallback m_OnCollisionEnter;
    CollisionCallback m_OnCollisionStay;
    CollisionCallback m_OnCollisionExit;
    CollisionCallback m_OnTriggerEnter;
    CollisionCallback m_OnTriggerStay;
    CollisionCallback m_OnTriggerExit;

private:
    void ProcessCollisionEvents();
    void RenderDebugInfo(HDC hdc);
    void CheckCollisionWithAll();

    // 모든 활성 콜라이더들을 추적하는 정적 리스트
    static vector<Collider*> s_AllColliders;

    bool m_ShowDebug;

public:
    // 정적 함수들
    static void RegisterCollider(Collider* collider);
    static void UnregisterCollider(Collider* collider);
    static const vector<Collider*>& GetAllColliders() { return s_AllColliders; }
};