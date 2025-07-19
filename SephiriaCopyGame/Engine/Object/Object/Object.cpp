#include <pch.h>
#include "Object.h"

Object::Object(const Object& other)
    : m_Name(other.m_Name + "_Clone")
    , m_Tag(other.m_Tag)
    , m_bisActive(other.m_bisActive)
    , m_Transform(other.m_Transform)
    , m_pParent(nullptr)
{
    CloneComponents(other);
    CloneChildren(other);
}

void Object::Update(float DeltaTime)
{
    if (!m_bisActive) return;

    // Start ȣ�� (�� ����)
    if (!m_bisStarted)
    {
        for (auto& component : m_Components)
        {
            if (component->IsEnabled())
                component->Init();
        }
        m_bisStarted = true;
    }

    // Update ȣ��
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

void Object::LateUpdate(float DeltaTime)
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

void Object::Render(HDC hdc)
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
unique_ptr<IPrototypeable> Object::Clone() const
{
    return make_unique<Object>(*this);
}

void Object::CopyFrom(const IPrototypeable* source)
{
    const Object* sourceObj = dynamic_cast<const Object*>(source);
    if (sourceObj)
    {
        *this = *sourceObj;
    }
}

// ���� �Ҵ� ������
Object& Object::operator=(const Object& other)
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

void Object::CloneComponents(const Object& other)
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

void Object::CloneChildren(const Object& other)
{
    for (const Object* child : other.m_Children)
    {
        auto clonedChild = make_unique<Object>(*child);
        // TODO - clonedChild->SetParent(this);
        m_Children.push_back(clonedChild.release());
    }
}

void Object::SetActive(bool bActive)
{
    m_bisActive = bActive;
}