#include <pch.h>
#include "PrototypeManager.h"


PrototypeManager::PrototypeManager()
{
}

PrototypeManager::~PrototypeManager()
{
}

// ������Ÿ�� ���
void PrototypeManager::RegisterPrototype(const string& name, unique_ptr<Object> prototype)
{
    m_Prototypes[name] = move(prototype);
}

// ������Ÿ�����κ��� �� �ν��Ͻ� ����
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

// ������Ÿ�� ���� Ȯ��
bool PrototypeManager::HasPrototype(const string& name) const
{
    return m_Prototypes.find(name) != m_Prototypes.end();
}

// ������Ÿ�� ����
void PrototypeManager::UnregisterPrototype(const string& name)
{
    m_Prototypes.erase(name);
}

// ��� ������Ÿ�� ����
vector<string> PrototypeManager::GetPrototypeNames() const
{
    vector<string> names;
    for (const auto& pair : m_Prototypes)
    {
        names.push_back(pair.first);
    }
    return names;
}
