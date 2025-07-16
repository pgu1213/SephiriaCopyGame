#pragma once
#include <Engine/Object/IPrototypeable/IPrototypeable.h>
#include <Engine/Object/Object/Object.h>

class Object;

class Component : public IPrototypeable
{
public:
    Component(Object* owner);
    virtual ~Component() = default; 
public:
    virtual Component* CloneImpl() const = 0;
    virtual void Init(); // 생성될 때 호출
    virtual void Update(); // 매 프레임 호출
    virtual void LateUpdate(); // 업데이트 후 호출
	virtual void Render(); // 렌더링 호출
	virtual void OnDestroy(); // 파괴될 때 호출
	virtual void OnEnable(); // 활성화될 때 호출
	virtual void OnDisable(); // 비활성화될 때 호출
public:
    unique_ptr<IPrototypeable> Clone() const override;
    void CopyFrom(const IPrototypeable* source) override;
    void SetOwner(Object* newOwner);
    bool IsEnabled() const { return m_bisEnabled; }
protected:
    Object* m_pOwner;
    bool m_bisEnabled;
};