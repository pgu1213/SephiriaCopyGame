#pragma once
#include <Engine/Object/Object/Object.h>

class Object;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

public:
    virtual void Init() {} // 씬 초기화 (파생 클래스에서 오버라이드)
    Object* CreateGameObject(const string& name = "GameObject");
    void DestroyGameObject(Object* obj);
    void Update(float DeltaTime);
    void Render(HDC hdc);
    void Clear(); // 모든 오브젝트 정리

private:
    void ProcessPendingObjects(); // 추가/삭제 대기 중인 오브젝트들 처리

private:
    vector<unique_ptr<Object>> m_GameObjects;
    queue<unique_ptr<Object>> m_ObjectsToAdd;
    vector<Object*> m_ObjectsToDestroy;
};