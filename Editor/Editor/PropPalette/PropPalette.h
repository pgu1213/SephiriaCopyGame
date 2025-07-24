#pragma once

class PropPalette
{
public:
    PropPalette();
    ~PropPalette();

public:
    void Initialize();
    void Update();
    void Render(HDC hdc);
    void Release();

    wstring GetSelectedProp() const { return m_SelectedProp; }

private:
    vector<wstring> m_PropList;
    wstring m_SelectedProp;
    int m_SelectedIndex;

    // UI ¼³Á¤
    int m_PaletteX;
    int m_PaletteY;
    int m_PaletteWidth;
    int m_PaletteHeight;
};