#include <pch.h>
#include "UIButton.h"
#include <Engine/Object/UI/UI.h>
#include <Engine/Managers/InputManager/InputManager.h>

UIButton::UIButton(UI* owner)
    : Component(owner)
    , m_CurrentState(ButtonState::Normal)
    , m_PreviousState(ButtonState::Normal)
    , m_IsInteractable(true)
    , m_IsPressed(false)
    , m_FontName(L"Arial")
    , m_FontSize(12)
    , m_IsBold(false)
    , m_TextColor(RGB(255, 255, 255))
    , m_NormalColor(RGB(100, 100, 100))
    , m_HoverColor(RGB(150, 150, 150))
    , m_PressedColor(RGB(80, 80, 80))
    , m_DisabledColor(RGB(64, 64, 64))
{
}

UIButton::UIButton(const UIButton& other)
    : Component(other)
    , m_CurrentState(other.m_CurrentState)
    , m_PreviousState(other.m_PreviousState)
    , m_IsInteractable(other.m_IsInteractable)
    , m_IsPressed(other.m_IsPressed)
    , m_Text(other.m_Text)
    , m_FontName(other.m_FontName)
    , m_FontSize(other.m_FontSize)
    , m_IsBold(other.m_IsBold)
    , m_TextColor(other.m_TextColor)
    , m_NormalColor(other.m_NormalColor)
    , m_HoverColor(other.m_HoverColor)
    , m_PressedColor(other.m_PressedColor)
    , m_DisabledColor(other.m_DisabledColor)
{
}

Component* UIButton::CloneImpl() const
{
    return new UIButton(*this);
}

void UIButton::CopyFrom(const IPrototypeable* source)
{
    Component::CopyFrom(source);

    const UIButton* sourceButton = dynamic_cast<const UIButton*>(source);
    if (sourceButton)
    {
        m_CurrentState = sourceButton->m_CurrentState;
        m_PreviousState = sourceButton->m_PreviousState;
        m_IsInteractable = sourceButton->m_IsInteractable;
        m_IsPressed = sourceButton->m_IsPressed;
        m_Text = sourceButton->m_Text;
        m_FontName = sourceButton->m_FontName;
        m_FontSize = sourceButton->m_FontSize;
        m_IsBold = sourceButton->m_IsBold;
        m_TextColor = sourceButton->m_TextColor;
        m_NormalColor = sourceButton->m_NormalColor;
        m_HoverColor = sourceButton->m_HoverColor;
        m_PressedColor = sourceButton->m_PressedColor;
		m_DisabledColor = sourceButton->m_DisabledColor;
    }
}

void UIButton::Init()
{
    Component::Init();
}

void UIButton::Update(float DeltaTime)
{
    if (!m_IsInteractable)
    {
        m_CurrentState = ButtonState::Disabled;
        return;
    }

    UpdateButtonState();
    HandleStateChange();
}

void UIButton::Render(HDC hdc)
{
    UI* ui = static_cast<UI*>(GetUIOwner());
    if (!ui) return;

    Vector2 worldPos = ui->GetWorldPosition();
    Vector2 size = ui->GetSize();

    // 버튼 영역 계산
    RECT buttonRect = {
        (LONG)(worldPos.x - size.x * 0.5f),
        (LONG)(worldPos.y - size.y * 0.5f),
        (LONG)(worldPos.x + size.x * 0.5f),
        (LONG)(worldPos.y + size.y * 0.5f)
    };

    // 배경 그리기
    COLORREF currentColor = GetCurrentColor();
    HBRUSH brush = CreateSolidBrush(currentColor);
    FillRect(hdc, &buttonRect, brush);
    DeleteObject(brush);

    // 테두리 그리기
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    // 텍스트 그리기
    if (!m_Text.empty())
    {
        HFONT font = CreateFont(
            m_FontSize, 0, 0, 0,
            m_IsBold ? FW_BOLD : FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            m_FontName.c_str()
        );

        HFONT oldFont = (HFONT)SelectObject(hdc, font);
        SetTextColor(m_TextColor);
        SetBkMode(hdc, TRANSPARENT);

        wstring wText(m_Text.begin(), m_Text.end());
        DrawText(hdc, wText.c_str(), -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont);
        DeleteObject(font);
    }
}

void UIButton::OnDestroy()
{
}

void UIButton::SetFont(const wstring& fontName, int fontSize, bool bold)
{
    m_FontName = fontName;
    m_FontSize = fontSize;
    m_IsBold = bold;
}

void UIButton::SetColors(COLORREF normal, COLORREF hover, COLORREF pressed, COLORREF disabled)
{
    m_NormalColor = normal;
    m_HoverColor = hover;
    m_PressedColor = pressed;
    m_DisabledColor = disabled;
}

void UIButton::UpdateButtonState()
{
    m_PreviousState = m_CurrentState;

    bool isMouseOver = IsMouseOverButton();
    bool isMousePressed = InputManager::GetInstance()->IsKeyPressed(VK_LBUTTON);
    bool isMouseDown = InputManager::GetInstance()->IsKeyDown(VK_LBUTTON);

    if (isMouseOver)
    {
        if (isMousePressed && !m_IsPressed)
        {
            m_IsPressed = true;
            m_CurrentState = ButtonState::Pressed;
        }
        else if (isMouseDown && m_IsPressed)
        {
            m_CurrentState = ButtonState::Pressed;
        }
        else if (!isMouseDown)
        {
            if (m_IsPressed)
            {
                m_IsPressed = false;
                // 클릭 이벤트 발생
                UI* ui = static_cast<UI*>(GetUIOwner());
                if (ui && ui->m_OnClick)
                {
                    ui->m_OnClick();
                }
            }
            m_CurrentState = ButtonState::Hover;
        }
    }
    else
    {
        m_IsPressed = false;
        m_CurrentState = ButtonState::Normal;
    }
}

void UIButton::HandleStateChange()
{
    if (m_PreviousState != m_CurrentState)
    {
        UI* ui = static_cast<UI*>(GetUIOwner());
        if (!ui) return;

        switch (m_CurrentState)
        {
        case ButtonState::Hover:
            if (m_PreviousState == ButtonState::Normal && ui->m_OnHover)
            {
                ui->m_OnHover();
            }
            break;
        case ButtonState::Normal:
            if (m_PreviousState == ButtonState::Hover && ui->m_OnHoverExit)
            {
                ui->m_OnHoverExit();
            }
            break;
        }
    }
}

bool UIButton::IsMouseOverButton() const
{
    UI* ui = static_cast<UI*>(GetUIOwner());
    if (!ui) return false;

    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    return ui->ContainsPoint(Vector2((float)mousePos.x, (float)mousePos.y));
}

COLORREF UIButton::GetCurrentColor() const
{
    switch (m_CurrentState)
    {
    case ButtonState::Hover:    return m_HoverColor;
    case ButtonState::Pressed:  return m_PressedColor;
    case ButtonState::Disabled: return m_DisabledColor;
    default:                    return m_NormalColor;
    }
}