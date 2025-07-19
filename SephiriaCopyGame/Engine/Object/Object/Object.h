#pragma once
#include <Engine/Object/IPrototypeable/IPrototypeable.h>
#include <Engine/Object/Component/Component.h>
#include <Struct.h>

class Component;

class Object : public IPrototypeable
{
public:
    Object() : m_Name("Empty"), m_Tag(""), m_bisActive(true), m_pParent(nullptr) {}
    explicit Object(const string& name) : m_Name(name), m_Tag(""), m_bisActive(true), m_pParent(nullptr) {}
    Object(const Object& other);
public:
    void Update(float DeltaTime);
    void LateUpdate(float DeltaTime);
    void Render(HDC hdc);
    Transform GetTransform() const { return m_Transform; }
public:
    unique_ptr<IPrototypeable> Clone() const override; // 프로토타입 패턴 구현
    void CopyFrom(const IPrototypeable* source) override;
    Object& operator=(const Object& other); // 복사 할당 연산자
    void SetActive(bool bActive);
private:
    void CloneComponents(const Object& other);
    void CloneChildren(const Object& other);
private:
    string m_Name;
    string m_Tag;
    bool m_bisActive;
	bool m_bisStarted = false;
    Transform m_Transform;
    Object* m_pParent;
    vector<Object*> m_Children;
    vector<unique_ptr<Component>> m_Components;
    unordered_map<type_index, Component*> m_ComponentMap;

public:
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        static_assert(is_base_of_v<Component, T>, "T는 Component로부터 상속받아야 합니다.");

        // 이미 존재하는 컴포넌트인지 확인
        if (HasComponent<T>())
            return GetComponent<T>();

        auto newComponent = make_unique<T>(this, forward<Args>(args)...);
        T* ptr = newComponent.get();

        m_Components.push_back(move(newComponent));
        m_ComponentMap[type_index(typeid(T))] = ptr;

        ptr->Init();
        return ptr;
    }

    template<typename T>
    T* GetComponent()
    {
        auto it = m_ComponentMap.find(type_index(typeid(T)));
        return (it != m_ComponentMap.end()) ? static_cast<T*>(it->second) : nullptr;
    }

    template<typename T>
    bool HasComponent()
    {
        return m_ComponentMap.find(type_index(typeid(T))) != m_ComponentMap.end();
    }

    template<typename T>
    void RemoveComponent()
    {
        auto it = m_ComponentMap.find(type_index(typeid(T)));
        if (it != m_ComponentMap.end())
        {
            Component* comp = it->second;
            comp->OnDestroy();

            m_Components.erase(
                remove_if(m_Components.begin(), m_Components.end(),
                    [comp](const unique_ptr<Component>& ptr) {
                        return ptr.get() == comp;
                    }),
                m_Components.end()
            );

            m_ComponentMap.erase(it);
        }
    }
};