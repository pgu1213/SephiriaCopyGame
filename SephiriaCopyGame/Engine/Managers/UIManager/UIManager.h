#pragma once
#include <Engine/Managers/SingletonManager/SingletonManager.h>
#include <Engine/Object/UI/UI.h>

class UIManager : public SingleTon<UIManager>
{
    friend class SingleTon<UIManager>;
private:
    explicit UIManager();
public:
    virtual ~UIManager();

public:
    void Init();
    void Update(float deltaTime);
    void Render(HDC hdc);
    void Release();

    // UI 추가/제거
    void AddUI(shared_ptr<UI> ui);
    void RemoveUI(shared_ptr<UI> ui);
    void ClearAllUI();

    // 마우스 이벤트 처리
    void HandleMouseMove(int x, int y);
    void HandleMouseDown(int x, int y);
    void HandleMouseUp(int x, int y);

    // 화면 크기 설정
    void SetScreenSize(float width, float height);

    // 포커스 관리
    void SetFocusedUI(UI* ui);
    UI* GetFocusedUI() const;

    // UI 검색
    shared_ptr<UI> FindUI(const function<bool(const UI*)>& predicate);
    vector<shared_ptr<UI>> FindAllUI(const function<bool(const UI*)>& predicate);

    // Z-Order 관리
    void BringToFront(shared_ptr<UI> ui);
    void SendToBack(shared_ptr<UI> ui);

private:

    // 마우스 이벤트를 처리할 UI 찾기 (최상위부터)
    shared_ptr<UI> FindUIUnderMouse(int x, int y);

private:
    vector<shared_ptr<UI>> m_UIList;    // 모든 UI 리스트
    UI* m_FocusedUI;                    // 현재 포커스된 UI
    bool m_NeedSort;                    // Z-Order 정렬이 필요한지 여부
};