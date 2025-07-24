#pragma once

class TilePalette
{
public:
    TilePalette();
    ~TilePalette();

public:
    void Initialize();
    void Update();
    void Render(HDC hdc);
    void Release();

    wstring GetSelectedTile() const { return m_SelectedTile; }
    void SetSelectedTile(const wstring& tileName) { m_SelectedTile = tileName; }

private:
    void LoadTileList();
    void HandleInput();
    void RenderTileList(HDC hdc);

private:
    vector<wstring> m_TileList;
    wstring m_SelectedTile;
    int m_ScrollOffset;
    int m_SelectedIndex;
    bool m_IsVisible;

    // UI ¼³Á¤
    int m_PaletteX;
    int m_PaletteY;
    int m_PaletteWidth;
    int m_PaletteHeight;
    int m_TileSize;
};