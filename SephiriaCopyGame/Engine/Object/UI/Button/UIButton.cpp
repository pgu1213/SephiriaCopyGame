#include <pch.h>
#include "UIButton.h"

UIButton::UIButton()
    : UI()
    , m_Text(L"Button")
    , m_FontName(L"Arial")
    , m_FontSize(16)
    , m_FontBold(false)
    , m_Font(nullptr)
    , m_TextAlignment(DT_CENTER | DT_VCENTER | DT_SINGLELINE)
    , m_BorderEnabled(true)
    , m_BorderWidth(1)
    , m_ImageOnly(false)
    , m_CurrentState(ButtonState::Normal)
{
    // �⺻ ���� ����
    m_TextColors[static_cast<int>(ButtonState::Normal)] = RGB(0, 0, 0);
    m_TextColors[static_cast<int>(ButtonState::Hovered)] = RGB(0, 0, 0);
    m_TextColors[static_cast<int>(ButtonState::Pressed)] = RGB(255, 255, 255);
    m_TextColors[static_cast<int>(ButtonState::Disabled)] = RGB(128, 128, 128);

    m_BackgroundColors[static_cast<int>(ButtonState::Normal)] = RGB(240, 240, 240);
    m_BackgroundColors[static_cast<int>(ButtonState::Hovered)] = RGB(220, 220, 220);
    m_BackgroundColors[static_cast<int>(ButtonState::Pressed)] = RGB(100, 100, 100);
    m_BackgroundColors[static_cast<int>(ButtonState::Disabled)] = RGB(200, 200, 200);

    m_BorderColors[static_cast<int>(ButtonState::Normal)] = RGB(128, 128, 128);
    m_BorderColors[static_cast<int>(ButtonState::Hovered)] = RGB(64, 64, 64);
    m_BorderColors[static_cast<int>(ButtonState::Pressed)] = RGB(32, 32, 32);
    m_BorderColors[static_cast<int>(ButtonState::Disabled)] = RGB(160, 160, 160);

    // �̹��� �ʱ�ȭ
    for (int i = 0; i < 4; ++i)
    {
        m_Images[i] = nullptr;
    }

    UICreateFont();
}

UIButton::UIButton(const UIButton& other)
    : UI(other)
    , m_Text(other.m_Text)
    , m_FontName(other.m_FontName)
    , m_FontSize(other.m_FontSize)
    , m_FontBold(other.m_FontBold)
    , m_Font(nullptr)
    , m_TextAlignment(other.m_TextAlignment)
    , m_BorderEnabled(other.m_BorderEnabled)
    , m_BorderWidth(other.m_BorderWidth)
    , m_ImageOnly(other.m_ImageOnly)
    , m_CurrentState(ButtonState::Normal)
{
    // ���� �迭 ����
    for (int i = 0; i < 4; ++i)
    {
        m_TextColors[i] = other.m_TextColors[i];
        m_BackgroundColors[i] = other.m_BackgroundColors[i];
        m_BorderColors[i] = other.m_BorderColors[i];
        m_Images[i] = other.m_Images[i]; // �̹����� ������ ���� (�������� ����)
    }

    UICreateFont();
}

UIButton::~UIButton()
{
    UIDestroyFont();
}

unique_ptr<IPrototypeable> UIButton::Clone() const
{
    return make_unique<UIButton>(*this);
}

void UIButton::CopyFrom(const IPrototypeable* source)
{
    UI::CopyFrom(source);

    const UIButton* button = dynamic_cast<const UIButton*>(source);
    if (button)
    {
        m_Text = button->m_Text;
        m_FontName = button->m_FontName;
        m_FontSize = button->m_FontSize;
        m_FontBold = button->m_FontBold;
        m_TextAlignment = button->m_TextAlignment;
        m_BorderEnabled = button->m_BorderEnabled;
        m_BorderWidth = button->m_BorderWidth;
        m_ImageOnly = button->m_ImageOnly;

        for (int i = 0; i < 4; ++i)
        {
            m_TextColors[i] = button->m_TextColors[i];
            m_BackgroundColors[i] = button->m_BackgroundColors[i];
            m_BorderColors[i] = button->m_BorderColors[i];
            m_Images[i] = button->m_Images[i];
        }

        UICreateFont();
    }
}

void UIButton::Render(HDC hdc)
{
    if (!m_Visible)
        return;

    // ���� ���� ������Ʈ
    if (!m_Enabled)
        m_CurrentState = ButtonState::Disabled;
    else if (m_IsPressed)
        m_CurrentState = ButtonState::Pressed;
    else if (m_IsHovered)
        m_CurrentState = ButtonState::Hovered;
    else
        m_CurrentState = ButtonState::Normal;

    // ��� ������
    RenderBackground(hdc);

    // �׵θ� ������
    if (m_BorderEnabled)
    {
        RenderBorder(hdc);
    }

    // �̹��� ������
    if (m_Images[static_cast<int>(m_CurrentState)])
    {
        RenderImage(hdc);
    }

    // �ؽ�Ʈ ������ (�̹��� ������ �ƴ� ���)
    if (!m_ImageOnly && !m_Text.empty())
    {
        RenderText(hdc);
    }

    // �ڽ� UI�� ������
    UI::Render(hdc);
}

void UIButton::SetText(const wstring& text)
{
    m_Text = text;
}

wstring UIButton::GetText() const
{
    return m_Text;
}

void UIButton::SetFont(const wstring& fontName, int fontSize, bool bold)
{
    m_FontName = fontName;
    m_FontSize = fontSize;
    m_FontBold = bold;
    UICreateFont();
}

void UIButton::SetFontSize(int fontSize)
{
    m_FontSize = fontSize;
    UICreateFont();
}

void UIButton::SetFontBold(bool bold)
{
    m_FontBold = bold;
    UICreateFont();
}

void UIButton::SetTextColor(COLORREF color)
{
    for (int i = 0; i < 4; ++i)
    {
        m_TextColors[i] = color;
    }
}

void UIButton::SetTextColor(ButtonState state, COLORREF color)
{
    m_TextColors[static_cast<int>(state)] = color;
}

COLORREF UIButton::GetTextColor(ButtonState state) const
{
    return m_TextColors[static_cast<int>(state)];
}

void UIButton::SetBackgroundColor(ButtonState state, COLORREF color)
{
    m_BackgroundColors[static_cast<int>(state)] = color;
}

COLORREF UIButton::GetBackgroundColor(ButtonState state) const
{
    return m_BackgroundColors[static_cast<int>(state)];
}

void UIButton::SetBorderEnabled(bool enabled)
{
    m_BorderEnabled = enabled;
}

bool UIButton::IsBorderEnabled() const
{
    return m_BorderEnabled;
}

void UIButton::SetBorderColor(COLORREF color)
{
    for (int i = 0; i < 4; ++i)
    {
        m_BorderColors[i] = color;
    }
}

void UIButton::SetBorderColor(ButtonState state, COLORREF color)
{
    m_BorderColors[static_cast<int>(state)] = color;
}

COLORREF UIButton::GetBorderColor(ButtonState state) const
{
    return m_BorderColors[static_cast<int>(state)];
}

void UIButton::SetBorderWidth(int width)
{
    m_BorderWidth = max(0, width);
}

int UIButton::GetBorderWidth() const
{
    return m_BorderWidth;
}

ButtonState UIButton::GetButtonState() const
{
    return m_CurrentState;
}

void UIButton::SetTextAlignment(int alignment)
{
    m_TextAlignment = alignment;
}

int UIButton::GetTextAlignment() const
{
    return m_TextAlignment;
}

void UIButton::SetImage(ButtonState state, Gdiplus::Bitmap* image)
{
    m_Images[static_cast<int>(state)] = image;
}

Gdiplus::Bitmap* UIButton::GetImage(ButtonState state) const
{
    return m_Images[static_cast<int>(state)];
}

void UIButton::SetImageOnly(bool imageOnly)
{
    m_ImageOnly = imageOnly;
}

bool UIButton::IsImageOnly() const
{
    return m_ImageOnly;
}

void UIButton::OnHover()
{
    UI::OnHover();
}

void UIButton::OnPress()
{
    UI::OnPress();
}

void UIButton::OnRelease()
{
    UI::OnRelease();
}

void UIButton::RenderBackground(HDC hdc)
{
    Vector2 screenPos = GetScreenPosition();
    Vector2 screenSize = GetActualSize();

    COLORREF bgColor = GetCurrentBackgroundColor();
    HBRUSH brush = CreateSolidBrush(bgColor);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    RECT rect;
    rect.left = static_cast<LONG>(screenPos.x);
    rect.top = static_cast<LONG>(screenPos.y);
    rect.right = static_cast<LONG>(screenPos.x + screenSize.x);
    rect.bottom = static_cast<LONG>(screenPos.y + screenSize.y);

    FillRect(hdc, &rect, brush);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}

void UIButton::RenderBorder(HDC hdc)
{
    Vector2 screenPos = GetScreenPosition();
    Vector2 screenSize = GetActualSize();

    COLORREF borderColor = GetCurrentBorderColor();
    HPEN pen = CreatePen(PS_SOLID, m_BorderWidth, borderColor);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    // �׵θ� �׸���
    for (int i = 0; i < m_BorderWidth; ++i)
    {
        Rectangle(hdc,
            static_cast<int>(screenPos.x + i),
            static_cast<int>(screenPos.y + i),
            static_cast<int>(screenPos.x + screenSize.x - i),
            static_cast<int>(screenPos.y + screenSize.y - i));
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void UIButton::RenderText(HDC hdc)
{
    if (m_Text.empty() || !m_Font)
        return;

    Vector2 screenPos = GetScreenPosition();
    Vector2 screenSize = GetActualSize();

    HFONT oldFont = (HFONT)SelectObject(hdc, m_Font);
    COLORREF oldTextColor = ::SetTextColor(hdc, GetCurrentTextColor());
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);

    RECT textRect;
    textRect.left = static_cast<LONG>(screenPos.x + m_BorderWidth);
    textRect.top = static_cast<LONG>(screenPos.y + m_BorderWidth);
    textRect.right = static_cast<LONG>(screenPos.x + screenSize.x - m_BorderWidth);
    textRect.bottom = static_cast<LONG>(screenPos.y + screenSize.y - m_BorderWidth);

    DrawText(hdc, m_Text.c_str(), -1, &textRect, m_TextAlignment);

    SetBkMode(hdc, oldBkMode);
    ::SetTextColor(hdc, oldTextColor);
    SelectObject(hdc, oldFont);
}

void UIButton::RenderImage(HDC hdc)
{
    Gdiplus::Bitmap* image = m_Images[static_cast<int>(m_CurrentState)];
    if (!image)
        return;

    Vector2 screenPos = GetScreenPosition();
    Vector2 screenSize = GetActualSize();

    Gdiplus::Graphics graphics(hdc);

    // �̹����� ��ư ũ�⿡ �°� �����ϸ��ؼ� �׸���
    Gdiplus::RectF destRect(
        screenPos.x,
        screenPos.y,
        screenSize.x,
        screenSize.y
    );

    graphics.DrawImage(image, destRect);
}

COLORREF UIButton::GetCurrentTextColor() const
{
    return m_TextColors[static_cast<int>(m_CurrentState)];
}

COLORREF UIButton::GetCurrentBackgroundColor() const
{
    return m_BackgroundColors[static_cast<int>(m_CurrentState)];
}

COLORREF UIButton::GetCurrentBorderColor() const
{
    return m_BorderColors[static_cast<int>(m_CurrentState)];
}

void UIButton::UICreateFont()
{
    UIDestroyFont();

    m_Font = CreateFontW(
        -m_FontSize,                    // ����
        0,                              // �ʺ� (0 = �⺻ ����)
        0,                              // ����
        0,                              // ����
        m_FontBold ? FW_BOLD : FW_NORMAL, // ����
        FALSE,                          // ���Ÿ�
        FALSE,                          // ����
        FALSE,                          // ��Ҽ�
        DEFAULT_CHARSET,                // ���ڼ�
        OUT_DEFAULT_PRECIS,             // ��� ���е�
        CLIP_DEFAULT_PRECIS,            // Ŭ���� ���е�
        DEFAULT_QUALITY,                // ǰ��
        DEFAULT_PITCH | FF_DONTCARE,    // ��ġ�� �йи�
        m_FontName.c_str()              // ��Ʈ �̸�
    );
}

void UIButton::UIDestroyFont()
{
    if (m_Font)
    {
        DeleteObject(m_Font);
        m_Font = nullptr;
    }
}