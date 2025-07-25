#pragma once
#include <Engine/Object/IPrototypeable/IPrototypeable.h>

class Component;

class UI : public IPrototypeable
{
public:
    UI() : m_Name("Empty"), m_Tag(""), m_bisActive(true), m_pParent(nullptr) {}
    explicit UI(const string& name) : m_Name(name), m_Tag(""), m_bisActive(true), m_pParent(nullptr) {}
    UI(const UI& other);
public:
    void Update(float DeltaTime);
    void LateUpdate(float DeltaTime);
    void Render(HDC hdc);
public: // Get / Set
    string GetName() const { return m_Name; }
    Transform GetTransform() const { return m_Transform; }
    void SetTransform(const Transform& transform) { m_Transform = transform; }
    void SetPosition(const Vector2& position) { m_Transform.position = position; }
    void SetPosition(float x, float y) { m_Transform.position = Vector2(x, y); }
    void SetRotation(float rotation) { m_Transform.rotation = rotation; }
    void SetScale(float x, float y) { m_Transform.scale = Vector2(x, y); }
    void SetScale(const Vector2& scale) { m_Transform.scale = scale; }

    // 활성화 상태
    bool IsActive() const { return m_bisActive; }
    void SetActive(bool active) { m_bisActive = active; }

    bool IsVisible() const { return m_bisVisible; }
    void SetVisible(bool visible) { m_bisVisible = visible; }
public:
    unique_ptr<IPrototypeable> Clone() const override; // 프로토타입 패턴 구현
    void CopyFrom(const IPrototypeable* source) override;
    UI& operator=(const UI& other); // 복사 할당 연산자
private:
    void CloneComponents(const UI& other);
    void CloneChildren(const UI& other);
private:
    string m_Name;
    string m_Tag;
    bool m_bisActive;
    bool m_bisVisible;
    bool m_bisStarted = false;
    Transform m_Transform;
    UI* m_pParent;
    vector<UI*> m_Children;
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
