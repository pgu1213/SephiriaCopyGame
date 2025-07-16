#include <pch.h>
#include "Component.h"

Component::Component(Object* owner) : m_pOwner(owner), m_bisEnabled(true)
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
        // owner는 복사 X (새로운 GameObject에 넣어야 함)
    }
}

void Component::SetOwner(Object* newOwner) { m_pOwner = newOwner; }

void Component::Init()
{
}

void Component::Update()
{
}

void Component::LateUpdate()
{
}

void Component::Render()
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
