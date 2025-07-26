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
    virtual void Init(); // ������ �� ȣ��
    virtual void Update(float deltaTime); // �� ������ ȣ��
    virtual void LateUpdate(float deltaTime); // ������Ʈ �� ȣ��
	virtual void Render(HDC hdc); // ���� ȣ��
	virtual void OnDestroy(); // �ı��� �� ȣ��
	virtual void OnEnable(); // Ȱ��ȭ�� �� ȣ��
	virtual void OnDisable(); // ��Ȱ��ȭ�� �� ȣ��
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