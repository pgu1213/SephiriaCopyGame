#pragma once
#include <Engine/Object/IPrototypeable/IPrototypeable.h>
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/UI/UI.h>

class Object;
class UI;

class Component : public IPrototypeable
{
public:
    Component(Object* owner);
    Component(UI* owner);
    virtual ~Component() = default; 
public:
    virtual Component* CloneImpl() const = 0;
    virtual void Init(); // 생성될 때 호출
    virtual void Update(float deltaTime); // 매 프레임 호출
    virtual void LateUpdate(float deltaTime); // 업데이트 후 호출
	virtual void Render(HDC hdc); // 렌더 호출
	virtual void OnDestroy(); // 파괴될 때 호출
	virtual void OnEnable(); // 활성화될 때 호출
	virtual void OnDisable(); // 비활성화될 때 호출
public:
    unique_ptr<IPrototypeable> Clone() const override;
    void CopyFrom(const IPrototypeable* source) override;
    void SetOwner(Object* newOwner);
	void SetOwner(UI* newOwner);
    bool IsEnabled() const { return m_bisEnabled; }
    Object* GetObjOwner() const { return m_pObjOwner; }
    UI* GetUIOwner() const { return m_pUIOwner; }
protected:
    Object* m_pObjOwner;
	UI* m_pUIOwner;
    bool m_bisEnabled;
};