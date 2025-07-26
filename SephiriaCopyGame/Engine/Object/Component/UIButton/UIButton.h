#pragma once
#include <Engine/Object/Component/Component.h>

class UIButton : public Component
{
private:
    ButtonState m_CurrentState;
    ButtonState m_PreviousState;
    bool m_IsInteractable;
    bool m_IsPressed;

    // 텍스트 관련
    string m_Text;
    wstring m_FontName;
    int m_FontSize;
    bool m_IsBold;

    // 색상 관련
    COLORREF m_TextColor;
    COLORREF m_NormalColor;
    COLORREF m_HoverColor;
    COLORREF m_PressedColor;
    COLORREF m_DisabledColor;

public:
    explicit UIButton(UI* owner);
    UIButton(const UIButton& other);
public:
    void Init() override;
    void Update(float DeltaTime) override;
    void Render(HDC hdc) override;
    void OnDestroy() override;
public:
    // 텍스트 설정
    void SetText(const string& text) { m_Text = text; }
    const string& GetText() const { return m_Text; }

    void SetFont(const wstring& fontName, int fontSize, bool bold = false);
    void SetTextColor(COLORREF color) { m_TextColor = color; }

    // 색상 설정
    void SetColors(COLORREF normal, COLORREF hover, COLORREF pressed, COLORREF disabled = RGB(128, 128, 128));

    // 상태 관리
    ButtonState GetState() const { return m_CurrentState; }
    void SetInteractable(bool interactable) { m_IsInteractable = interactable; }
    bool IsInteractable() const { return m_IsInteractable; }

private:
    void UpdateButtonState();
    void HandleStateChange();
    bool IsMouseOverButton() const;
    COLORREF GetCurrentColor() const;
};