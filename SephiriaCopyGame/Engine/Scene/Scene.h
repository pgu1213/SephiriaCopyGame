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
    virtual void Init() {} // 씬 초기화 (파생 클래스에서 오버라이드)
    void Update(float DeltaTime);
    void Render(HDC hdc);
    void Clear(); // 모든 오브젝트 정리
    //오브젝트 관련
    Object* CreateGameObject(const string& name = "GameObject");
    void DestroyGameObject(Object* obj);
    Object* FindGameObject(const string& name) const; // 이름으로 오브젝트 찾기
    //UI 관련
    UI* CreateUI(const string& name = "UI");
    void DestroyUI(UI* obj);
    UI* FindUI(const string& name) const; // 이름으로 오브젝트 찾기

private:
    void ProcessPendingObjects(); // 추가/삭제 대기 중인 오브젝트들 처리
    void ProcessPendingUI();

private:
    vector<unique_ptr<Object>> m_GameObjects;
    queue<unique_ptr<Object>> m_ObjectsToAdd;
    vector<Object*> m_ObjectsToDestroy;
    vector<unique_ptr<UI>> m_UI;
    queue<unique_ptr<UI>> m_UIToAdd;
    vector<UI*> m_UIToDestroy;
};