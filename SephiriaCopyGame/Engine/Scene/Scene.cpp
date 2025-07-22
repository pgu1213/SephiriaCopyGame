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
    // 대기 중인 오브젝트들 처리
    ProcessPendingObjects();

    // 모든 게임 오브젝트 업데이트
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
    // 모든 게임 오브젝트 업데이트
    for (auto& obj : m_GameObjects)
    {
        if (obj)
        {
            obj->Render(hdc);
        }
    }
}

void Scene::Clear()
{
    // 모든 오브젝트 제거
    m_GameObjects.clear();

    // 대기 중인 오브젝트들도 제거
    while (!m_ObjectsToAdd.empty())
    {
        m_ObjectsToAdd.pop();
    }

    m_ObjectsToDestroy.clear();
}

Object* Scene::FindGameObject(const string& name) const
{
    // 이름으로 오브젝트 찾기
    for (const auto& obj : m_GameObjects)
    {
        if (obj && obj->GetName() == name)
        {
            return obj.get();
        }
    }
	return nullptr; // 찾지 못한 경우 nullptr 반환
}

void Scene::ProcessPendingObjects()
{
    // 동적 추가 시 대기 중인 오브젝트 처리
    while (!m_ObjectsToAdd.empty())
    {
        m_GameObjects.push_back(move(m_ObjectsToAdd.front()));
        m_ObjectsToAdd.pop();
    }

    // 동적 삭제 대기 중인 오브젝트들 처리
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