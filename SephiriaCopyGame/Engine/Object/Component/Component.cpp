#include <pch.h>
#include "Component.h"

Component::Component(Object* owner) : m_pObjOwner(owner), m_bisEnabled(true)
{
}

Component::Component(UI* owner) : m_pUIOwner(owner), m_bisEnabled(true)
{
}

// 프로토타입 클론
unique_ptr<IPrototypeable> Component::Clone() const
{
    return unique_ptr<Component>(CloneImpl());
}

void Component::CopyFrom(const IPrototypeable* source)
{
    const Component* sourceComp = dynamic_cast<const Component*>(source);
    if (sourceComp)
    {
        m_bisEnabled = sourceComp->m_bisEnabled;
        // owner는 복사 X (새로 만든 GameObject에 넣어야 함)
    }
}

void Component::SetOwner(Object* newOwner) { m_pObjOwner = newOwner; }
void Component::SetOwner(UI* newOwner) { m_pUIOwner = newOwner; }

void Component::Init()
{
}

void Component::Update(float deltaTime)
{
}

void Component::LateUpdate(float deltaTime)
{
}

void Component::Render(HDC hdc)
{
}

void Component::OnDestroy()
{
}

void Component::OnEnable()
{
}

void Component::OnDisable()
{
}
