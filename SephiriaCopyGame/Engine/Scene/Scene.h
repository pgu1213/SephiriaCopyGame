#pragma once
#include <Engine/Object/Object/Object.h>

class Object;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

public:
    virtual void Init() {} // �� �ʱ�ȭ (�Ļ� Ŭ�������� �������̵�)
    Object* CreateGameObject(const string& name = "GameObject");
    void DestroyGameObject(Object* obj);
    void Update(float DeltaTime);
    void Render(HDC hdc);
    void Clear(); // ��� ������Ʈ ����

private:
    void ProcessPendingObjects(); // �߰�/���� ��� ���� ������Ʈ�� ó��

private:
    vector<unique_ptr<Object>> m_GameObjects;
    queue<unique_ptr<Object>> m_ObjectsToAdd;
    vector<Object*> m_ObjectsToDestroy;
};