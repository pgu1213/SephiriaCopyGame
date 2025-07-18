#include <pch.h>
#include "Scene.h"

Object* Scene::CreateGameObject(const string& name)
{
    auto obj = make_unique<Object>(name);
    Object* rawPtr = obj.get();
    m_ObjectsToAdd.push(move(obj));
    return rawPtr;
}

void Scene::DestroyGameObject(Object* obj)
{
    if (obj)
    {
        m_ObjectsToDestroy.push_back(obj);
    }
}

void Scene::Update(float DeltaTime)
{
    // ��� ���� ������Ʈ�� ó��
    ProcessPendingObjects();

    // ��� ���� ������Ʈ ������Ʈ
    for (auto& obj : m_GameObjects)
    {
        if (obj)
        {
            obj->Update(DeltaTime);
        }
    }
}

void Scene::Clear()
{
    // ��� ������Ʈ ����
    m_GameObjects.clear();

    // ��� ���� ������Ʈ�鵵 ����
    while (!m_ObjectsToAdd.empty())
    {
        m_ObjectsToAdd.pop();
    }

    m_ObjectsToDestroy.clear();
}

void Scene::ProcessPendingObjects()
{
    // �߰� ��� ���� ������Ʈ�� ó��
    while (!m_ObjectsToAdd.empty())
    {
        m_GameObjects.push_back(move(m_ObjectsToAdd.front()));
        m_ObjectsToAdd.pop();
    }

    // ���� ��� ���� ������Ʈ�� ó��
    for (Object* objToDestroy : m_ObjectsToDestroy)
    {
        m_GameObjects.erase(
            remove_if(m_GameObjects.begin(), m_GameObjects.end(),
                [objToDestroy](const unique_ptr<Object>& obj) {
                    return obj.get() == objToDestroy;
                }),
            m_GameObjects.end()
        );
    }
    m_ObjectsToDestroy.clear();
}