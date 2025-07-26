#include <pch.h>
#include "Collider.h"
#include <Engine/Managers/SceneManager/SceneManager.h>
#include <Engine/Object/Component/Camera/Camera.h>

vector<Collider*> Collider::s_AllColliders;

Collider::Collider(Object* owner) 
    : Component(owner)
    , m_Offset(0, 0)
    , m_IsTrigger(false)
    , m_Layer(CollisionLayer::None)
    , m_LayerMask(CollisionLayer::All)
	, m_Enabled(true)
    , m_ShowDebug(true)
{
    RegisterCollider(this);
}

Collider::~Collider()
{
    UnregisterCollider(this);

    // 다른 콜라이더들의 충돌 목록에서 자신을 제거
    for (Collider* other : s_AllColliders)
    {
        if (other != this)
        {
            other->m_PreviousCollisions.erase(this);
            other->m_CurrentCollisions.erase(this);
        }
    }
}

void Collider::Update(float DeltaTime)
{
    if (!IsEnabled()) return;

    SyncWithOwnerTransform();

    // 이전 프레임의 충돌 정보를 저장
    m_PreviousCollisions = m_CurrentCollisions;
    m_CurrentCollisions.clear();

    // 모든 다른 콜라이더와 충돌 검사
    CheckCollisionWithAll();

    // 이벤트 처리
    ProcessCollisionEvents();
}

void Collider::Render(HDC hdc)
{
    if (m_ShowDebug)
    {
        RenderDebugInfo(hdc);
    }
}

void Collider::CheckCollisionWithAll()
{
    for (Collider* other : s_AllColliders)
    {
        if (other == this || !other->IsEnabled()) continue;
        if (!CanCollideWith(other->GetLayer())) continue;

        if (CheckCollision(other))
        {
            m_CurrentCollisions.insert(other);
        }
    }
}

void Collider::ProcessCollisionEvents()
{
    // Enter 이벤트 (이전에 없었는데 현재 있는 것)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) == m_PreviousCollisions.end())
        {
            // 새로운 충돌
            if (m_IsTrigger)
            {
                if (m_OnTriggerEnter)
                {
                    m_OnTriggerEnter(collider);
                }
            }
            else
            {
                if (m_OnCollisionEnter)
                {
                    m_OnCollisionEnter(collider);
                }
            }
        }
    }

    // Stay 이벤트 (이전에도 있었고 현재도 있는 것)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) != m_PreviousCollisions.end())
        {
            // 지속 중인 충돌
            if (m_IsTrigger)
            {
                if (m_OnTriggerStay)
                {
                    m_OnTriggerStay(collider);
                }
            }
            else
            {
                if (m_OnCollisionStay)
                {
                    m_OnCollisionStay(collider);
                }
            }
        }
    }

    // Exit 이벤트 (이전에 있었는데 현재 없는 것)
    for (Collider* collider : m_PreviousCollisions)
    {
        if (m_CurrentCollisions.find(collider) == m_CurrentCollisions.end())
        {
            // 충돌 끝남
            if (m_IsTrigger)
            {
                if (m_OnTriggerExit)
                {
                    m_OnTriggerExit(collider);
                }
            }
            else
            {
                if (m_OnCollisionExit)
                {
                    m_OnCollisionExit(collider);
                }
            }
        }
    }
}

void Collider::SyncWithOwnerTransform()
{
    if (m_pObjOwner)
    {
        auto ownerTransform = m_pObjOwner->GetTransform();

        // 콜라이더의 위치를 오너의 위치로 업데이트
        m_Position.x = static_cast<float>(ownerTransform.position.x);
        m_Position.y = static_cast<float>(ownerTransform.position.y);

        // 오프셋이 있다면 적용
        m_Position.x += m_Offset.x;
        m_Position.y += m_Offset.y;

        // 회전도 동기화 (필요한 경우)
        m_Rotation = ownerTransform.rotation;

        // 스케일도 동기화 (필요한 경우)
        m_Scale = ownerTransform.scale;
    }
}

void Collider::RenderDebugInfo(HDC hdc)
{
    if (!m_pObjOwner) return;

    // 카메라 가져오기
    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* cameraObject = currentScene->FindGameObject("MainCamera");
    Camera* camera = cameraObject->GetComponent<Camera>();

    if (!camera) return;

    Rect bounds = GetScreenBounds(camera);

    RenderColliderBounds(hdc, bounds, camera);
    RenderCollisionState(hdc, bounds, camera);
    RenderTriggerState(hdc, bounds, camera);

}

bool Collider::CanCollideWith(CollisionLayer otherLayer) const
{
    return static_cast<uint32_t>(m_LayerMask) & static_cast<uint32_t>(otherLayer);
}

void Collider::RegisterCollider(Collider* collider)
{
    s_AllColliders.push_back(collider);
}

void Collider::UnregisterCollider(Collider* collider)
{
    auto it = 
        find(s_AllColliders.begin(), s_AllColliders.end(), collider);
    if (it != s_AllColliders.end())
    {
        s_AllColliders.erase(it);
    }
}

Rect Collider::GetScreenBounds(Camera* camera)
{
    Vector2 size = GetSize(); // 각 콜라이더에서 구현

    // 월드 좌표에서 화면 좌표로 변환
    POINT topLeftScreen = camera->WorldToScreen(
        m_Position.x - size.x * 0.5f,
        m_Position.y - size.y * 0.5f
    );

    POINT bottomRightScreen = camera->WorldToScreen(
        m_Position.x + size.x * 0.5f,
        m_Position.y + size.y * 0.5f
    );

    return Rect(
        topLeftScreen.x,
        topLeftScreen.y,
        bottomRightScreen.x - topLeftScreen.x,
        bottomRightScreen.y - topLeftScreen.y
    );
}

bool Collider::IsVisibleOnScreen(Camera* camera, const Rect& screenBounds)
{
    // 화면 크기 가져오기 (카메라나 윈도우에서)
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    // 화면 영역과 겹치는지 확인
    return !(screenBounds.X + screenBounds.Width < 0 ||
        screenBounds.X > clientRect.right ||
        screenBounds.Y + screenBounds.Height < 0 ||
        screenBounds.Y > clientRect.bottom);
}

void Collider::RenderColliderBounds(HDC hdc, const Rect& screenBounds, Camera* camera)
{
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc,
        screenBounds.X,
        screenBounds.Y,
        screenBounds.X + screenBounds.Width,
        screenBounds.Y + screenBounds.Height);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
}

void Collider::RenderCollisionState(HDC hdc, const Rect& screenBounds, Camera* camera)
{
    if (!m_CurrentCollisions.empty())
    {
        // 충돌 중인 경우 초록색 테두리
        HPEN collisionPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
        HPEN oldPen = (HPEN)SelectObject(hdc, collisionPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

        Rectangle(hdc,
            screenBounds.X - 2,
            screenBounds.Y - 2,
            screenBounds.X + screenBounds.Width + 2,
            screenBounds.Y + screenBounds.Height + 2);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(collisionPen);
    }
}

void Collider::RenderTriggerState(HDC hdc, const Rect& screenBounds, Camera* camera)
{
    if (m_IsTrigger)
    {
        HPEN triggerPen = CreatePen(PS_DASH, 1, RGB(0, 0, 255));
        HPEN oldPen = (HPEN)SelectObject(hdc, triggerPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

        Rectangle(hdc,
            screenBounds.X,
            screenBounds.Y,
            screenBounds.X + screenBounds.Width,
            screenBounds.Y + screenBounds.Height);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(triggerPen);
    }
}