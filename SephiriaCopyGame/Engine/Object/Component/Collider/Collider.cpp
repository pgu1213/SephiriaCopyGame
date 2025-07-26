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

    // �ٸ� �ݶ��̴����� �浹 ��Ͽ��� �ڽ��� ����
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

    // ���� �������� �浹 ������ ����
    m_PreviousCollisions = m_CurrentCollisions;
    m_CurrentCollisions.clear();

    // ��� �ٸ� �ݶ��̴��� �浹 �˻�
    CheckCollisionWithAll();

    // �̺�Ʈ ó��
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
    // Enter �̺�Ʈ (������ �����µ� ���� �ִ� ��)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) == m_PreviousCollisions.end())
        {
            // ���ο� �浹
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

    // Stay �̺�Ʈ (�������� �־��� ���絵 �ִ� ��)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) != m_PreviousCollisions.end())
        {
            // ���� ���� �浹
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

    // Exit �̺�Ʈ (������ �־��µ� ���� ���� ��)
    for (Collider* collider : m_PreviousCollisions)
    {
        if (m_CurrentCollisions.find(collider) == m_CurrentCollisions.end())
        {
            // �浹 ����
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

        // �ݶ��̴��� ��ġ�� ������ ��ġ�� ������Ʈ
        m_Position.x = static_cast<float>(ownerTransform.position.x);
        m_Position.y = static_cast<float>(ownerTransform.position.y);

        // �������� �ִٸ� ����
        m_Position.x += m_Offset.x;
        m_Position.y += m_Offset.y;

        // ȸ���� ����ȭ (�ʿ��� ���)
        m_Rotation = ownerTransform.rotation;

        // �����ϵ� ����ȭ (�ʿ��� ���)
        m_Scale = ownerTransform.scale;
    }
}

void Collider::RenderDebugInfo(HDC hdc)
{
    if (!m_pObjOwner) return;

    // ī�޶� ��������
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
    Vector2 size = GetSize(); // �� �ݶ��̴����� ����

    // ���� ��ǥ���� ȭ�� ��ǥ�� ��ȯ
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
    // ȭ�� ũ�� �������� (ī�޶� �����쿡��)
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    // ȭ�� ������ ��ġ���� Ȯ��
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
        // �浹 ���� ��� �ʷϻ� �׵θ�
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