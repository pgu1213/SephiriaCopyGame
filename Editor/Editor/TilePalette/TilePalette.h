#pragma once
#include "../../Managers/ResourceManager/ResourceManager.h"

class TileMapEditor;

class TilePalette
{
public:
    TilePalette();
    ~TilePalette();

public:
    void Init(TileMapEditor* tileMapEditor);
    void Update();
    void Render(HDC hdc);

    void SetPosition(int x, int y);
    void SetSize(int width, int height);

    // Ÿ�� ����
    void SelectTile(int index);
    int GetSelectedTileIndex() const { return m_SelectedTileIndex; }

    // ������ ��� (Ÿ���� ���� ���)
    void NextPage();
    void PrevPage();
    void SetTilesPerPage(int tilesPerPage);

private:
    static const int DEFAULT_TILE_SIZE = 32; // �ȷ�Ʈ������ Ÿ�� ũ��
    static const int TILES_PER_ROW = 8;
    static const int TILES_PER_PAGE = 32;

    TileMapEditor* m_pTileMapEditor;

    // �ȷ�Ʈ ��ġ �� ũ��
    int m_PosX;
    int m_PosY;
    int m_Width;
    int m_Height;

    // Ÿ�� ���� ����
    int m_SelectedTileIndex;
    int m_CurrentPage;
    int m_TilesPerPage;

    // Ÿ�� ���ϸ� ���
    vector<wstring> m_TileFileNames;

    void HandleInput();
    void DrawTileGrid(HDC hdc);
    void DrawSelectedHighlight(HDC hdc);
    void DrawPageInfo(HDC hdc);
    void DrawPageButtons(HDC hdc, int totalPages);
    bool CheckPageButtons(int mouseX, int mouseY);

    // ��ǥ ��ȯ
    int GetTileIndexFromPosition(int x, int y);
    RECT GetTileRect(int index);
};