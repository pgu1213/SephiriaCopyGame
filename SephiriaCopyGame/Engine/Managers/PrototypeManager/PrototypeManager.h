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
    void RegisterPrototype(const string& name, unique_ptr<Object> prototype); // 프로토타입 등록
    unique_ptr<Object> CreateFromPrototype(const string& name); // 프로토타입으로부터 새 인스턴스 생성
    bool HasPrototype(const string& name) const; // 프로토타입 존재 확인
    void UnregisterPrototype(const string& name); // 프로토타입 제거
    vector<string> GetPrototypeNames() const; // 모든 프로토타입 반환
private:
    unordered_map<string, unique_ptr<Object>> m_Prototypes;
};