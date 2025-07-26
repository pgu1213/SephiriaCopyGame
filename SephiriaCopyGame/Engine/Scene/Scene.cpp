#include <pch.h>
#include "Scene.h"

void Scene::Update(float DeltaTime)
{
    // ��� ���� ������Ʈ�� ó��
    ProcessPendingUI();
    ProcessPendingObjects();

    // ��� ���� ������Ʈ ������Ʈ
    for (auto& obj : m_GameObjects)
    {
        if (obj)
        {
            obj->Update(DeltaTime);
        }
    }
    // UI ������Ʈ ������Ʈ
    for (auto& ui : m_UI)
    {
        if (ui)
        {
            ui->Update(DeltaTime);
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
    // UI ������Ʈ ������
    for (auto& ui : m_UI)
    {
        if (ui)
        {
            ui->Render(hdc);
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

void Scene::ProcessPendingUI()
{
    // ���� �߰� �� ��� ���� ������Ʈ ó��
    while (!m_UIToAdd.empty())
    {
        m_UI.push_back(move(m_UIToAdd.front()));
        m_UIToAdd.pop();
    }

    // ���� ���� ��� ���� ������Ʈ�� ó��
    for (UI* uiToDestroy : m_UIToDestroy)
    {
        m_UI.erase(
            remove_if(m_UI.begin(), m_UI.end(),
                [uiToDestroy](const unique_ptr<UI>& ui) {
                    return ui.get() == uiToDestroy;
                }),
            m_UI.end()
        );
    }
    m_UIToDestroy.clear();
}

void Scene::Clear()
{
    // UI ����
	m_UI.clear();

    // ��� ������Ʈ ����
    m_GameObjects.clear();

    // ��� ���� UI ����
    while (!m_UIToAdd.empty())
    {
        m_UIToAdd.pop();
    }

    // ��� ���� ������Ʈ�鵵 ����
    while (!m_ObjectsToAdd.empty())
    {
        m_ObjectsToAdd.pop();
    }

    m_UIToDestroy.clear();
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
    auto ui = make_unique<UI>(name);
    UI* rawPtr = ui.get();
    m_UIToAdd.push(move(ui));
    return rawPtr;
}

void Scene::DestroyUI(UI* ui)
{
    if (ui)
    {
        m_UIToDestroy.push_back(ui);
    }
}

UI* Scene::FindUI(const string& name) const
{
    // �̸����� ������Ʈ ã��
    for (const auto& ui : m_UI)
    {
        if (ui && ui->GetName() == name)
        {
            return ui.get();
        }
    }
    return nullptr; // ã�� ���� ��� nullptr ��ȯ
}

