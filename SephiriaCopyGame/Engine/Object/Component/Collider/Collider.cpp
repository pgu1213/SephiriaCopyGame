#include <pch.h>
#include <functional>
#include "Collider.h"

vector<Collider*> Collider::s_AllColliders;

Collider::Collider(Object* owner) 
    : Component(owner)
    , m_Offset(0, 0)
    , m_IsTrigger(false)
    , m_Layer(CollisionLayer::None)
    , m_LayerMask(CollisionLayer::All)
    , m_ShowDebug(false)
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

    // ���� �������� �浹 ������ ����
    m_PreviousCollisions = m_CurrentCollisions;
    m_CurrentCollisions.clear();

    // ��� �ٸ� �ݶ��̴��� �浹 �˻�
    CheckCollisionWithAll();

    // �̺�Ʈ ó��
    ProcessCollisionEvents();
}

void Collider::Render(HDC hdc)  // override Ű����� �������
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
                m_OnTriggerEnter(collider);
            else
                m_OnCollisionEnter(collider);
        }
    }

    // Stay �̺�Ʈ (�������� �־��� ���絵 �ִ� ��)
    for (Collider* collider : m_CurrentCollisions)
    {
        if (m_PreviousCollisions.find(collider) != m_PreviousCollisions.end())
        {
            // ���� ���� �浹
            if (m_IsTrigger)
                m_OnTriggerStay(collider);
            else
                m_OnCollisionStay(collider);
        }
    }

    // Exit �̺�Ʈ (������ �־��µ� ���� ���� ��)
    for (Collider* collider : m_PreviousCollisions)
    {
        if (m_CurrentCollisions.find(collider) == m_CurrentCollisions.end())
        {
            // �浹 ����
            if (m_IsTrigger)
                m_OnTriggerExit(collider);
            else
                m_OnCollisionExit(collider);
        }
    }
}

void Collider::RenderDebugInfo(HDC hdc)
{
    Rect bounds = GetBounds();

    // �⺻ �ݶ��̴� ��� (������)
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, bounds.X, bounds.Y,
        bounds.X + bounds.Width, bounds.Y + bounds.Height);

    // �浹 ���� ��� �ʷϻ����� ǥ��
    if (!m_CurrentCollisions.empty())
    {
        HPEN collisionPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
        SelectObject(hdc, collisionPen);

        Rectangle(hdc, bounds.X - 2, bounds.Y - 2,
            bounds.X + bounds.Width + 2, bounds.Y + bounds.Height + 2);

        DeleteObject(collisionPen);
    }

    // Ʈ������ ��� �Ķ��� �������� ǥ��
    if (m_IsTrigger)
    {
        HPEN triggerPen = CreatePen(PS_DASH, 1, RGB(0, 0, 255));
        SelectObject(hdc, triggerPen);

        Rectangle(hdc, bounds.X, bounds.Y,
            bounds.X + bounds.Width, bounds.Y + bounds.Height);

        DeleteObject(triggerPen);
    }

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
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