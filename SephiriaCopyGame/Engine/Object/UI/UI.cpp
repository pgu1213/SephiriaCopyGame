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

    // �ڽ� ������Ʈ ������Ʈ
    for (auto* child : m_Children)
    {
        child->Update(DeltaTime);
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

// ������Ÿ�� Ŭ�� ����
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

// ���� �Ҵ� ������
UI& UI::operator=(const UI& other)
{
    if (this != &other)
    {
        m_Name = other.m_Name + "_Clone";
        m_Tag = other.m_Tag;
        m_bisActive = other.m_bisActive;
        m_Transform = other.m_Transform;
        m_pParent = nullptr;

        // ������ ������ �ִ� ������Ʈ�� ����
        m_Components.clear();
        m_ComponentMap.clear();

        // ���� �ڽĵ� ����
        for (auto* child : m_Children)
        {
            delete child;
        }
        m_Children.clear();

        // ���ο� ������ ����
        CloneComponents(other);
        CloneChildren(other);
    }
    return *this;
}

void UI::CloneComponents(const UI& other)
{
    for (const auto& component : other.m_Components)
    {
        // ���ο� ������ ����
        auto clonedComponent = component->Clone();
        Component* ptr = static_cast<Component*>(clonedComponent.get());
        ptr->SetOwner(this);

        // ������ ����
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