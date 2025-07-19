#pragma once

struct TilePaletteItem
{
    int tileID;
    wstring fileName;
    Gdiplus::Bitmap* bitmap;
    RECT displayRect;
};

class TilePalette
{
public:
    TilePalette();
    ~TilePalette();

public:
    void Initialize();
    void Update();
    void Render(HDC hdc);
    void HandleInput();

    int GetSelectedTileID() const { return m_SelectedTileID; }
    wstring GetSelectedTileFileName() const;
    void SetPosition(int x, int y) { m_PosX = x; m_PosY = y; }
    void SetSize(int width, int height) { m_Width = width; m_Height = height; }

private:
    void LoadTileItems();
    void UpdateLayout();
    void RenderTileGrid(HDC hdc);
    void RenderSelection(HDC hdc);
    int GetTileIndexAt(int x, int y);

private:
    vector<TilePaletteItem> m_TileItems;
    int m_SelectedTileID;
    int m_SelectedIndex;

    // UI 包访
    int m_PosX, m_PosY;
    int m_Width, m_Height;
    int m_TileDisplaySize;
    int m_TilesPerRow;
    int m_ScrollOffset;

    // 胶农费 包访
    int m_MaxScrollOffset;
    bool m_NeedUpdateLayout;
};