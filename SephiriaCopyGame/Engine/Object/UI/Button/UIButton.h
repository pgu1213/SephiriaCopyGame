#pragma once
#include "../UI.h"

// ��ư ����
enum class ButtonState
{
    Normal,
    Hovered,
    Pressed,
    Disabled
};

class UIButton : public UI
{
public:
    UIButton();
    UIButton(const UIButton& other);
    virtual ~UIButton();

public:
    // IPrototypeable �������̽�
    unique_ptr<IPrototypeable> Clone() const override;
    void CopyFrom(const IPrototypeable* source) override;

public:
	void Update(float deltaTime) override;
    void Render(HDC hdc) override;

    // �ؽ�Ʈ ����
    void SetText(const wstring& text);
    wstring GetText() const;

    // ��Ʈ ����
    void SetFont(const wstring& fontName, int fontSize, bool bold = false);
    void SetFontSize(int fontSize);
    void SetFontBold(bool bold);

    // �ؽ�Ʈ ���� ���� (���º�)
    void SetTextColor(COLORREF color);
    void SetTextColor(ButtonState state, COLORREF color);
    COLORREF GetTextColor(ButtonState state) const;

    // ��� ���� ���� (���º�)
    void SetBackgroundColor(ButtonState state, COLORREF color);
    COLORREF GetBackgroundColor(ButtonState state) const;

    // �׵θ� ����
    void SetBorderEnabled(bool enabled);
    bool IsBorderEnabled() const;
    void SetBorderColor(COLORREF color);
    void SetBorderColor(ButtonState state, COLORREF color);
    COLORREF GetBorderColor(ButtonState state) const;
    void SetBorderWidth(int width);
    int GetBorderWidth() const;

    // ��ư ����
    ButtonState GetButtonState() const;

    // �ؽ�Ʈ ����
    void SetTextAlignment(int alignment); // DT_CENTER, DT_LEFT, DT_RIGHT ��
    int GetTextAlignment() const;

    // �̹��� ��ư ����
    void SetImage(ButtonState state, Gdiplus::Bitmap* image);
    Gdiplus::Bitmap* GetImage(ButtonState state) const;
    void SetImageOnly(bool imageOnly); // �ؽ�Ʈ ���� �̹����� ǥ��
    bool IsImageOnly() const;

protected:
    // UI �̺�Ʈ �������̵�
    void OnHover() override;
    void OnPress() override;
    void OnRelease() override;

    // ������ ���� �Լ���
    void RenderBackground(HDC hdc);
    void RenderBorder(HDC hdc);
    void RenderText(HDC hdc);
    void RenderImage(HDC hdc);

    // ���� ���¿� ���� ���� ��������
    COLORREF GetCurrentTextColor() const;
    COLORREF GetCurrentBackgroundColor() const;
    COLORREF GetCurrentBorderColor() const;

    // ��Ʈ ����/����
    void UICreateFont();
    void UIDestroyFont();

private:
    wstring m_Text;                     // ��ư �ؽ�Ʈ
    wstring m_FontName;                 // ��Ʈ �̸�
    int m_FontSize;                     // ��Ʈ ũ��
    bool m_FontBold;                    // ��Ʈ ����
    HFONT m_Font;                       // ��Ʈ �ڵ�
    int m_TextAlignment;                // �ؽ�Ʈ ����

    // ���º� ����
    COLORREF m_TextColors[4];           // �ؽ�Ʈ ���� (Normal, Hovered, Pressed, Disabled)
    COLORREF m_BackgroundColors[4];     // ��� ����
    COLORREF m_BorderColors[4];         // �׵θ� ����

    // �׵θ� ����
    bool m_BorderEnabled;               // �׵θ� ��� ����
    int m_BorderWidth;                  // �׵θ� �β�

    // �̹��� ����
    Gdiplus::Bitmap* m_Images[4];       // ���º� �̹���
    bool m_ImageOnly;                   // �̹����� ǥ�� ����

    // ���� ��ư ����
    ButtonState m_CurrentState;
};