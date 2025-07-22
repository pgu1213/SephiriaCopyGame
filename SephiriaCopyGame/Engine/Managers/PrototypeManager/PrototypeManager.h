#pragma once
#include <Engine/Managers/SingletonManager/SingletonManager.h>
#include <Engine/Object/Object/Object.h>

class PrototypeManager : public SingleTon<PrototypeManager>
{
friend class SingleTon<PrototypeManager>;
private:
    explicit PrototypeManager();
public:
    virtual ~PrototypeManager();
public:
    void RegisterPrototype(const string& name, unique_ptr<Object> prototype); // ������Ÿ�� ���
    unique_ptr<Object> CreateFromPrototype(const string& name); // ������Ÿ�����κ��� �� �ν��Ͻ� ����
    bool HasPrototype(const string& name) const; // ������Ÿ�� ���� Ȯ��
    void UnregisterPrototype(const string& name); // ������Ÿ�� ����
    vector<string> GetPrototypeNames() const; // ��� ������Ÿ�� ��ȯ
private:
    unordered_map<string, unique_ptr<Object>> m_Prototypes;
};