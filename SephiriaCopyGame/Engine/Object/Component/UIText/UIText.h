#pragma once
#include <Engine/Object/Component/Component.h>

enum class TextAlignment
{
    Left,
    Center,
    Right
};

class UIText : public Component
{
private:
    string m_Text;
    wstring m_FontName;
    int m_FontSize;
    bool m_IsBold;
    bool m_IsItalic;
    bool m_IsUnderline;
    COLORREF m_TextColor;
    TextAlignment m_Alignment;

public:
    explicit UIText(UI* owner);
    UIText(const UIText& other);
    virtual ~UIText() = default;

public:
    void Init() override;
    void Render(HDC hdc) override;

public:
    // 텍스트 설정
    void SetText(const string& text) { m_Text = text; }
    const string& GetText() const { return m_Text; }

    // 폰트 설정
    void SetFont(const wstring& fontName, int fontSize, bool bold = false, bool italic = false, bool underline = false);
    void SetTextColor(COLORREF color) { m_TextColor = color; }

    // 정렬 설정
    void SetAlignment(TextAlignment alignment) { m_Alignment = alignment; }
    TextAlignment GetAlignment() const { return m_Alignment; }
};