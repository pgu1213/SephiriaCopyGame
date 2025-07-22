#include <pch.h>
#include "Scene.h"

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

void Scene::Render(HDC hdc)
{
    // ��� ���� ������Ʈ ������Ʈ
    for (auto& obj : m_GameObjects)
    {
        if (obj)
        {
            obj->Render(hdc);
        }
    }
}

void Scene::ProcessPendingObjects()
{
    // ���� �߰� �� ��� ���� ������Ʈ ó��
    while (!m_ObjectsToAdd.empty())
    {
        m_GameObjects.push_back(move(m_ObjectsToAdd.front()));
        m_ObjectsToAdd.pop();
    }

    // ���� ���� ��� ���� ������Ʈ�� ó��
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

Object* Scene::FindGameObject(const string& name) const
{
    // �̸����� ������Ʈ ã��
    for (const auto& obj : m_GameObjects)
    {
        if (obj && obj->GetName() == name)
        {
            return obj.get();
        }
    }
	return nullptr; // ã�� ���� ��� nullptr ��ȯ
}

UI* Scene::CreateUI(const string& name)
{
    auto obj = make_unique<UI>(name);
    UI* rawPtr = obj.get();
    m_UIToAdd.push(move(obj));
    return rawPtr;
}

void Scene::DestroyUI(UI* obj)
{
    if (obj)
    {
        m_UIToDestroy.push_back(obj);
    }
}

UI* Scene::FindUI(const string& name) const
{
    // �̸����� ������Ʈ ã��
    for (const auto& obj : m_UI)
    {
        if (obj && obj->GetName() == name)
        {
            return obj.get();
        }
    }
    return nullptr; // ã�� ���� ��� nullptr ��ȯ
}

