#include <pch.h>
#include "PrototypeManager.h"


PrototypeManager::PrototypeManager()
{
}

PrototypeManager::~PrototypeManager()
{
}

// 프로토타입 등록
void PrototypeManager::RegisterPrototype(const string& name, unique_ptr<Object> prototype)
{
    m_Prototypes[name] = move(prototype);
}

// 프로토타입으로부터 새 인스턴스 생성
unique_ptr<Object> PrototypeManager::CreateFromPrototype(const string& name)
{
    auto it = m_Prototypes.find(name);
    if (it != m_Prototypes.end())
    {
        return unique_ptr<Object>(
            static_cast<Object*>(it->second->Clone().release())
        );
    }
    return nullptr;
}

// 프로토타입 존재 확인
bool PrototypeManager::HasPrototype(const string& name) const
{
    return m_Prototypes.find(name) != m_Prototypes.end();
}

// 프로토타입 제거
void PrototypeManager::UnregisterPrototype(const string& name)
{
    m_Prototypes.erase(name);
}

// 모든 프로토타입 나열
vector<string> PrototypeManager::GetPrototypeNames() const
{
    vector<string> names;
    for (const auto& pair : m_Prototypes)
    {
        names.push_back(pair.first);
    }
    return names;
}
