#include "pch.h"
#include "UIText.h"
#include <Engine/Object/UI/UI.h>

UIText::UIText(UI* owner)
    : Component(owner)
    , m_FontName(L"Arial")
    , m_FontSize(12)
    , m_IsBold(false)
    , m_IsItalic(false)
    , m_IsUnderline(false)
    , m_TextColor(RGB(255, 255, 255))
    , m_Alignment(TextAlignment::Left)
{
}

UIText::UIText(const UIText& other)
    : Component(other)
    , m_Text(other.m_Text)
    , m_FontName(other.m_FontName)
    , m_FontSize(other.m_FontSize)
    , m_IsBold(other.m_IsBold)
    , m_IsItalic(other.m_IsItalic)
    , m_IsUnderline(other.m_IsUnderline)
    , m_TextColor(other.m_TextColor)
    , m_Alignment(other.m_Alignment)
{
}

void UIText::Init()
{
    Component::Init();
}

void UIText::Render(HDC hdc)
{
    if (m_Text.empty()) return;

    UI* ui = static_cast<UI*>(GetUIOwner());
    if (!ui) return;

    Vector2 worldPos = ui->GetWorldPosition();
    Vector2 size = ui->GetSize();

    // 텍스트 영역 계산
    RECT textRect = {
        (LONG)(worldPos.x - size.x * 0.5f),
        (LONG)(worldPos.y - size.y * 0.5f),
        (LONG)(worldPos.x + size.x * 0.5f),
        (LONG)(worldPos.y + size.y * 0.5f)
    };

    // 폰트 생성
    HFONT font = CreateFont(
        m_FontSize, 0, 0, 0,
        m_IsBold ? FW_BOLD : FW_NORMAL,
        m_IsItalic, m_IsUnderline, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        m_FontName.c_str()
    );

    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(m_TextColor);
    SetBkMode(hdc, TRANSPARENT);

    // 정렬 플래그 설정
    UINT format = DT_VCENTER | DT_SINGLELINE;
    switch (m_Alignment)
    {
    case TextAlignment::Left:   format |= DT_LEFT; break;
    case TextAlignment::Center: format |= DT_CENTER; break;
    case TextAlignment::Right:  format |= DT_RIGHT; break;
    }

    // 텍스트 그리기
    wstring wText(m_Text.begin(), m_Text.end());
    DrawText(hdc, wText.c_str(), -1, &textRect, format);

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void UIText::SetFont(const wstring& fontName, int fontSize, bool bold, bool italic, bool underline)
{
    m_FontName = fontName;
    m_FontSize = fontSize;
    m_IsBold = bold;
    m_IsItalic = italic;
    m_IsUnderline = underline;
}