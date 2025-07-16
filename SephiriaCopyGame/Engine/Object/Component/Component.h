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
    virtual void Init(); // ������ �� ȣ��
    virtual void Update(); // �� ������ ȣ��
    virtual void LateUpdate(); // ������Ʈ �� ȣ��
	virtual void Render(); // ������ ȣ��
	virtual void OnDestroy(); // �ı��� �� ȣ��
	virtual void OnEnable(); // Ȱ��ȭ�� �� ȣ��
	virtual void OnDisable(); // ��Ȱ��ȭ�� �� ȣ��
public:
    unique_ptr<IPrototypeable> Clone() const override;
    void CopyFrom(const IPrototypeable* source) override;
    void SetOwner(Object* newOwner);
    bool IsEnabled() const { return m_bisEnabled; }
protected:
    Object* m_pOwner;
    bool m_bisEnabled;
};