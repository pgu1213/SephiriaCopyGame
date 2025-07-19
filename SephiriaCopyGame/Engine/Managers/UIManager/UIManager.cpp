#include <pch.h>
#include "UIManager.h"

UIManager::UIManager()
    : m_FocusedUI(nullptr)
    , m_NeedSort(false)
{
}

UIManager::~UIManager()
{
    Release();
}

void UIManager::Init()
{
    m_UIList.clear();
    m_FocusedUI = nullptr;
    m_NeedSort = false;

    // �⺻ ȭ�� ũ�� ����
    SetScreenSize(static_cast<float>(WINCX), static_cast<float>(WINCY));
}

void UIManager::Update(float deltaTime)
{
    // Z-Order ������ �ʿ��� ��� ����
    if (m_NeedSort)
    {
        SortUIsByZOrder();
        m_NeedSort = false;
    }

    // ��� UI ������Ʈ
    for (auto& ui : m_UIList)
    {
        if (ui)
        {
            ui->Update(deltaTime);
        }
    }

    // ��ȿ���� ���� UI ����
    m_UIList.erase(
        remove_if(m_UIList.begin(), m_UIList.end(),
            [](const weak_ptr<UI>& weak_ui) {
                return weak_ui.expired();
            }),
        m_UIList.end()
    );
}

void UIManager::Render(HDC hdc)
{
    // Z-Order ������ ������ (���� ������ ���� �� ��)
    for (auto& ui : m_UIList)
    {
        if (ui)
        {
            ui->Render(hdc);
        }
    }
}

void UIManager::Release()
{
    m_UIList.clear();
    m_FocusedUI = nullptr;
}

void UIManager::AddUI(shared_ptr<UI> ui)
{
    if (ui)
    {
        m_UIList.push_back(ui);
        m_NeedSort = true;
    }
}

void UIManager::RemoveUI(shared_ptr<UI> ui)
{
    auto it = find(m_UIList.begin(), m_UIList.end(), ui);
    if (it != m_UIList.end())
    {
        if (m_FocusedUI == ui.get())
        {
            m_FocusedUI = nullptr;
        }
        m_UIList.erase(it);
    }
}

void UIManager::ClearAllUI()
{
    m_UIList.clear();
    m_FocusedUI = nullptr;
}

void UIManager::HandleMouseMove(int x, int y)
{
    // �������� �˻� (�ֻ��� UI����)
    for (auto it = m_UIList.rbegin(); it != m_UIList.rend(); ++it)
    {
        if (*it && (*it)->OnMouseMove(x, y))
        {
            break; // �̺�Ʈ�� ó���� UI�� ������ �ߴ�
        }
    }
}

void UIManager::HandleMouseDown(int x, int y)
{
    shared_ptr<UI> clickedUI = FindUIUnderMouse(x, y);

    if (clickedUI)
    {
        SetFocusedUI(clickedUI.get());
        clickedUI->OnMouseDown(x, y);
    }
    else
    {
        SetFocusedUI(nullptr);
    }
}

void UIManager::HandleMouseUp(int x, int y)
{
    // �������� �˻� (�ֻ��� UI����)
    for (auto it = m_UIList.rbegin(); it != m_UIList.rend(); ++it)
    {
        if (*it && (*it)->OnMouseUp(x, y))
        {
            break; // �̺�Ʈ�� ó���� UI�� ������ �ߴ�
        }
    }
}

void UIManager::SetScreenSize(float width, float height)
{
    UI::SetScreenSize(width, height);
}

void UIManager::SetFocusedUI(UI* ui)
{
    m_FocusedUI = ui;
}

UI* UIManager::GetFocusedUI() const
{
    return m_FocusedUI;
}

shared_ptr<UI> UIManager::FindUI(const function<bool(const UI*)>& predicate)
{
    for (auto& ui : m_UIList)
    {
        if (ui && predicate(ui.get()))
        {
            return ui;
        }
    }
    return nullptr;
}

vector<shared_ptr<UI>> UIManager::FindAllUI(const function<bool(const UI*)>& predicate)
{
    vector<shared_ptr<UI>> result;

    for (auto& ui : m_UIList)
    {
        if (ui && predicate(ui.get()))
        {
            result.push_back(ui);
        }
    }

    return result;
}

void UIManager::BringToFront(shared_ptr<UI> ui)
{
    if (!ui)
        return;

    // ���� ���� Z-Order �� ã��
    int maxZOrder = 0;
    for (auto& existingUI : m_UIList)
    {
        if (existingUI && existingUI != ui)
        {
            maxZOrder = max(maxZOrder, existingUI->GetZOrder());
        }
    }

    // �ش� UI�� Z-Order�� �ְ� + 1�� ����
    ui->SetZOrder(maxZOrder + 1);
    m_NeedSort = true;
}

void UIManager::SendToBack(shared_ptr<UI> ui)
{
    if (!ui)
        return;

    // ���� ���� Z-Order �� ã��
    int minZOrder = 0;
    for (auto& existingUI : m_UIList)
    {
        if (existingUI && existingUI != ui)
        {
            minZOrder = min(minZOrder, existingUI->GetZOrder());
        }
    }

    // �ش� UI�� Z-Order�� ������ - 1�� ����
    ui->SetZOrder(minZOrder - 1);
    m_NeedSort = true;
}

void UIManager::SortUIsByZOrder()
{
    sort(m_UIList.begin(), m_UIList.end(),
        [](const shared_ptr<UI>& a, const shared_ptr<UI>& b) {
            if (!a || !b) return false;
            return a->GetZOrder() < b->GetZOrder();
        });
}

shared_ptr<UI> UIManager::FindUIUnderMouse(int x, int y)
{
    // �������� �˻� (�ֻ��� UI����)
    for (auto it = m_UIList.rbegin(); it != m_UIList.rend(); ++it)
    {
        if (*it && (*it)->IsVisible() && (*it)->IsEnabled() && (*it)->IsPointInside(x, y))
        {
            return *it;
        }
    }
    return nullptr;
}