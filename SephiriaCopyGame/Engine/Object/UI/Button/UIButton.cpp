#include <pch.h>
#include "UIButton.h"
#include <Engine/Object/UI/UI.h>
#include <Engine/Managers/InputManager/InputManager.h>

UIButtonComponent::UIButtonComponent(UI* owner)
    : Component(owner)
    , m_CurrentState(ButtonState::Normal)
    , m_PreviousState(ButtonState::Normal)
    , m_TextColor(Color(255, 255, 255, 255))
    , m_BackgroundColor(Color(100, 100, 100, 255))
    , m_IsInteractable(true)
{
}

UIButtonComponent::UIButtonComponent(const UIButtonComponent& other)
    : Component(other)
    , m_OnClick(other.m_OnClick)
    , m_OnHover(other.m_OnHover)
    , m_OnHoverExit(other.m_OnHoverExit)
    , m_CurrentState(other.m_CurrentState)
    , m_PreviousState(other.m_PreviousState)
    , m_Text(other.m_Text)
    , m_TextColor(other.m_TextColor)
    , m_BackgroundColor(other.m_BackgroundColor)
    , m_IsInteractable(other.m_IsInteractable)
{
}

Component* UIButtonComponent::CloneImpl() const
{
    return new UIButtonComponent(*this);
}

void UIButtonComponent::Update(float deltaTime)
{
    if (!m_IsInteractable)
    {
        m_CurrentState = ButtonState::Disabled;
        return;
    }

    UpdateButtonState();
    HandleStateChange();
}

void UIButtonComponent::Render(HDC hdc)
{
    UI* ui = static_cast<UI*>(GetOwner());
    if (!ui) return;

    Vector2 position = ui->GetPosition();
    Vector2 size = ui->GetSize();

    // 버튼 배경 그리기
    RECT buttonRect = {
        (LONG)(position.x - size.x * 0.5f),
        (LONG)(position.y - size.y * 0.5f),
        (LONG)(position.x + size.x * 0.5f),
        (LONG)(position.y + size.y * 0.5f)
    };

    // 상태에 따른 색상 변경
    Color currentColor = m_BackgroundColor;
    switch (m_CurrentState)
    {
    case ButtonState::Hover:
        currentColor = Color(
            min(255, m_BackgroundColor.r + 30),
            min(255, m_BackgroundColor.g + 30),
            min(255, m_BackgroundColor.b + 30),
            m_BackgroundColor.a
        );
        break;
    case ButtonState::Pressed:
        currentColor = Color(
            max(0, m_BackgroundColor.r - 30),
            max(0, m_BackgroundColor.g - 30),
            max(0, m_BackgroundColor.b - 30),
            m_BackgroundColor.a
        );
        break;
    case ButtonState::Disabled:
        currentColor = Color(
            m_BackgroundColor.r / 2,
            m_BackgroundColor.g / 2,
            m_BackgroundColor.b / 2,
            m_BackgroundColor.a / 2
        );
        break;
    }

    HBRUSH brush = CreateSolidBrush(RGB(currentColor.r, currentColor.g, currentColor.b));
    FillRect(hdc, &buttonRect, brush);
    DeleteObject(brush);

    // 텍스트 그리기
    if (!m_Text.empty())
    {
        SetTextColor(hdc, RGB(m_TextColor.r, m_TextColor.g, m_TextColor.b));
        SetBkMode(hdc, TRANSPARENT);

        std::wstring wText(m_Text.begin(), m_Text.end());
        DrawText(hdc, wText.c_str(), -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void UIButtonComponent::UpdateButtonState()
{
    m_PreviousState = m_CurrentState;

    bool isMouseOver = IsMouseOverButton();
    bool isMousePressed = InputManager::GetInstance()->IsMousePressed(0); // 좌클릭
    bool isMouseDown = InputManager::GetInstance()->IsMouseDown(0);

    if (isMouseOver)
    {
        if (isMousePressed)
        {
            m_CurrentState = ButtonState::Pressed;
        }
        else if (isMouseDown)
        {
            m_CurrentState = ButtonState::Pressed;
        }
        else
        {
            m_CurrentState = ButtonState::Hover;
        }
    }
    else
    {
        m_CurrentState = ButtonState::Normal;
    }
}

void UIButtonComponent::HandleStateChange()
{
    // 상태 변화 처리
    if (m_PreviousState != m_CurrentState)
    {
        switch (m_CurrentState)
        {
        case ButtonState::Hover:
            if (m_PreviousState == ButtonState::Normal && m_OnHover)
            {
                m_OnHover();
            }
            break;
        case ButtonState::Normal:
            if (m_PreviousState == ButtonState::Hover && m_OnHoverExit)
            {
                m_OnHoverExit();
            }
            break;
        }
    }

    // 클릭 처리
    if (m_CurrentState == ButtonState::Pressed &&
        m_PreviousState == ButtonState::Hover &&
        m_OnClick)
    {
        m_OnClick();
    }
}

bool UIButtonComponent::IsMouseOverButton() const
{
    UI* ui = static_cast<UI*>(GetOwner());
    if (!ui) return false;

    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    return ui->ContainsPoint(Vector2((float)mousePos.x, (float)mousePos.y));
}