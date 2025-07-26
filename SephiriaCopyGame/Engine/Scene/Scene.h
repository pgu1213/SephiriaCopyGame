#pragma once
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/UI/UI.h>

class Object;
class UI;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

public:
    virtual void Init() {} // �� �ʱ�ȭ (�Ļ� Ŭ�������� �������̵�)
    void Update(float DeltaTime);
    void Render(HDC hdc);
    void Clear(); // ��� ������Ʈ ����
    //������Ʈ ����
    Object* CreateGameObject(const string& name = "GameObject");
    void DestroyGameObject(Object* obj);
    Object* FindGameObject(const string& name) const; // �̸����� ������Ʈ ã��
    //UI ����
    UI* CreateUI(const string& name = "UI");
    void DestroyUI(UI* obj);
    UI* FindUI(const string& name) const; // �̸����� ������Ʈ ã��

private:
    void ProcessPendingObjects(); // �߰�/���� ��� ���� ������Ʈ�� ó��
    void ProcessPendingUI();

private:
    vector<unique_ptr<Object>> m_GameObjects;
    queue<unique_ptr<Object>> m_ObjectsToAdd;
    vector<Object*> m_ObjectsToDestroy;
    vector<unique_ptr<UI>> m_UI;
    queue<unique_ptr<UI>> m_UIToAdd;
    vector<UI*> m_UIToDestroy;
};