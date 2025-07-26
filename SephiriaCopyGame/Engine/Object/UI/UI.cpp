#include <pch.h>
#include "UI.h"
#include <Engine/Object/Component/Component.h>

UI::UI(const UI& other)
    : m_Name(other.m_Name + "_Clone")
    , m_Tag(other.m_Tag)
    , m_bisActive(other.m_bisActive)
    , m_Transform(other.m_Transform)
    , m_pParent(nullptr)
{
    CloneComponents(other);
    CloneChildren(other);
}

void UI::Update(float DeltaTime)
{
    if (!m_bisActive) return;

    if (!m_bisStarted)
    {
        for (auto& component : m_Components)
        {
            if (component->IsEnabled())
                component->Init();
        }
        m_bisStarted = true;
    }

    for (auto& component : m_Components)
    {
        if (component->IsEnabled())
            component->Update(DeltaTime);
    }

    // 자식 오브젝트 업데이트
    for (auto* child : m_Children)
    {
        if (child)
        {
            child->Update(DeltaTime);
        }
    }
}

void UI::LateUpdate(float DeltaTime)
{
    if (!m_bisActive) return;

    for (auto& component : m_Components)
    {
        if (component->IsEnabled())
            component->LateUpdate(DeltaTime);
    }

    for (auto* child : m_Children)
    {
        child->LateUpdate(DeltaTime);
    }
}

void UI::Render(HDC hdc)
{
    if (!m_bisActive) return;

    for (auto& component : m_Components)
    {
        if (component->IsEnabled())
            component->Render(hdc);
    }

    for (auto* child : m_Children)
    {
        child->Render(hdc);
    }
}

// 프로토타입 클론 생성
unique_ptr<IPrototypeable> UI::Clone() const
{
    return make_unique<UI>(*this);
}

void UI::CopyFrom(const IPrototypeable* source)
{
    const UI* sourceObj = dynamic_cast<const UI*>(source);
    if (sourceObj)
    {
        *this = *sourceObj;
    }
}

// 복사 할당 연산자
UI& UI::operator=(const UI& other)
{
    if (this != &other)
    {
        m_Name = other.m_Name + "_Clone";
        m_Tag = other.m_Tag;
        m_bisActive = other.m_bisActive;
        m_Transform = other.m_Transform;
        m_pParent = nullptr;

        // 기존에 가지고 있던 컴포넌트는 정리
        m_Components.clear();
        m_ComponentMap.clear();

        // 기존 자식도 정리
        for (auto* child : m_Children)
        {
            delete child;
        }
        m_Children.clear();

        // 새로운 데이터 복사
        CloneComponents(other);
        CloneChildren(other);
    }
    return *this;
}

void UI::CloneComponents(const UI& other)
{
    for (const auto& component : other.m_Components)
    {
        // 새로운 소유자 설정
        auto clonedComponent = component->Clone();
        Component* ptr = static_cast<Component*>(clonedComponent.get());
        ptr->SetOwner(this);

        // 소유권 이전
        m_Components.push_back(unique_ptr<Component>(ptr));
        m_ComponentMap[type_index(typeid(*ptr))] = ptr;
        clonedComponent.release();
    }
}

void UI::CloneChildren(const UI& other)
{
    for (const UI* child : other.m_Children)
    {
        auto clonedChild = make_unique<UI>(*child);
        // TODO - clonedChild->SetParent(this);
        m_Children.push_back(clonedChild.release());
    }
}

void UI::SetParent(UI* parent)
{
    if (m_pParent == parent) return;

    if (m_pParent)
    {
        m_pParent->RemoveChild(this);
    }

    m_pParent = parent;

    if (m_pParent)
    {
        m_pParent->AddChild(this);
    }
}

void UI::AddChild(UI* child)
{
    if (!child || child == this) return;

    auto it = find(m_Children.begin(), m_Children.end(), child);
    if (it == m_Children.end())
    {
        m_Children.push_back(child);
        child->m_pParent = this;
    }
}

void UI::RemoveChild(UI* child)
{
    auto it = find(m_Children.begin(), m_Children.end(), child);
    if (it != m_Children.end())
    {
        (*it)->m_pParent = nullptr;
        m_Children.erase(it);
    }
}

Vector2 UI::GetWorldPosition() const
{
    if (m_pParent)
    {
        return m_pParent->LocalToWorld(m_Transform.position);
    }
    return m_Transform.position;
}

Vector2 UI::LocalToWorld(const Vector2& localPos) const
{
    Vector2 worldPos = localPos;

    // 스케일 적용
    worldPos.x *= m_Transform.scale.x;
    worldPos.y *= m_Transform.scale.y;

    // 회전 적용
    if (m_Transform.rotation != 0.0f)
    {
        float cos_r = cosf(m_Transform.rotation);
        float sin_r = sinf(m_Transform.rotation);
        float x = worldPos.x * cos_r - worldPos.y * sin_r;
        float y = worldPos.x * sin_r + worldPos.y * cos_r;
        worldPos.x = x;
        worldPos.y = y;
    }

    // 위치 적용
    worldPos += m_Transform.position;

    // 부모가 있으면 부모의 변환 적용
    if (m_pParent)
    {
        worldPos = m_pParent->LocalToWorld(worldPos);
    }

    return worldPos;
}

Vector2 UI::WorldToLocal(const Vector2& worldPos) const
{
    Vector2 localPos = worldPos;

    // 부모가 있으면 먼저 부모의 역변환 적용
    if (m_pParent)
    {
        localPos = m_pParent->WorldToLocal(localPos);
    }

    // 위치 역변환
    localPos -= m_Transform.position;

    // 회전 역변환
    if (m_Transform.rotation != 0.0f)
    {
        float cos_r = cosf(-m_Transform.rotation);
        float sin_r = sinf(-m_Transform.rotation);
        float x = localPos.x * cos_r - localPos.y * sin_r;
        float y = localPos.x * sin_r + localPos.y * cos_r;
        localPos.x = x;
        localPos.y = y;
    }

    // 스케일 역변환
    localPos.x /= m_Transform.scale.x;
    localPos.y /= m_Transform.scale.y;

    return localPos;
}

bool UI::ContainsPoint(const Vector2& point) const
{
    Vector2 localPoint = WorldToLocal(point);

    return localPoint.x >= -m_Size.x * 0.5f &&
        localPoint.x <= m_Size.x * 0.5f &&
        localPoint.y >= -m_Size.y * 0.5f &&
        localPoint.y <= m_Size.y * 0.5f;
}