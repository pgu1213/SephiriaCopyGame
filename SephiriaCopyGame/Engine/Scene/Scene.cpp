#include <pch.h>
#include "Scene.h"

Object* Scene::CreateGameObject(const string& name)
{
    auto gameObject = make_unique<Object>(name);
    Object* ptr = gameObject.get();

    m_ObjectsToAdd.push(move(gameObject));
    return ptr;
}

void Scene::DestroyGameObject(Object* obj)
{
    if (obj)
    {
        m_ObjectsToDestroy.push_back(obj);
        obj->SetActive(false);
    }
}

void Scene::Update()
{
    // 새 오브젝트 추가
    while (!m_ObjectsToAdd.empty())
    {
        m_GameObjects.push_back(move(m_ObjectsToAdd.front()));
        m_ObjectsToAdd.pop();
    }

    // 게임 오브젝트 업데이트
    for (auto& obj : m_GameObjects)
    {
        obj->Update();
    }

    for (auto& obj : m_GameObjects)
    {
        obj->LateUpdate();
    }

    // 파괴된 오브젝트 제거
    for (auto* obj : m_ObjectsToDestroy)
    {
        m_GameObjects.erase
        (
            remove_if(m_GameObjects.begin(), m_GameObjects.end(),
                [obj](const std::unique_ptr<Object>& ptr)
                {
                    return ptr.get() == obj;
                }),
            m_GameObjects.end()
        );
    }
    m_ObjectsToDestroy.clear();
}
