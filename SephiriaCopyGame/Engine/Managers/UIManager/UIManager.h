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

    // UI �߰�/����
    void AddUI(shared_ptr<UI> ui);
    void RemoveUI(shared_ptr<UI> ui);
    void ClearAllUI();

    // ���콺 �̺�Ʈ ó��
    void HandleMouseMove(int x, int y);
    void HandleMouseDown(int x, int y);
    void HandleMouseUp(int x, int y);

    // ȭ�� ũ�� ����
    void SetScreenSize(float width, float height);

    // ��Ŀ�� ����
    void SetFocusedUI(UI* ui);
    UI* GetFocusedUI() const;

    // UI �˻�
    shared_ptr<UI> FindUI(const function<bool(const UI*)>& predicate);
    vector<shared_ptr<UI>> FindAllUI(const function<bool(const UI*)>& predicate);

    // Z-Order ����
    void BringToFront(shared_ptr<UI> ui);
    void SendToBack(shared_ptr<UI> ui);

private:

    // ���콺 �̺�Ʈ�� ó���� UI ã�� (�ֻ�������)
    shared_ptr<UI> FindUIUnderMouse(int x, int y);

private:
    vector<shared_ptr<UI>> m_UIList;    // ��� UI ����Ʈ
    UI* m_FocusedUI;                    // ���� ��Ŀ���� UI
    bool m_NeedSort;                    // Z-Order ������ �ʿ����� ����
};