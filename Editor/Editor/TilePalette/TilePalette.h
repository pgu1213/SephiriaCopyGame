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

    // 타일 선택
    void SelectTile(int index);
    int GetSelectedTileIndex() const { return m_SelectedTileIndex; }

    // 페이지 기능 (타일이 많을 경우)
    void NextPage();
    void PrevPage();
    void SetTilesPerPage(int tilesPerPage);

private:
    static const int DEFAULT_TILE_SIZE = 32; // 팔레트에서의 타일 크기
    static const int TILES_PER_ROW = 8;
    static const int TILES_PER_PAGE = 32;

    TileMapEditor* m_pTileMapEditor;

    // 팔레트 위치 및 크기
    int m_PosX;
    int m_PosY;
    int m_Width;
    int m_Height;

    // 타일 선택 관련
    int m_SelectedTileIndex;
    int m_CurrentPage;
    int m_TilesPerPage;

    // 타일 파일명 목록
    vector<wstring> m_TileFileNames;

    void HandleInput();
    void DrawTileGrid(HDC hdc);
    void DrawSelectedHighlight(HDC hdc);
    void DrawPageInfo(HDC hdc);
    void DrawPageButtons(HDC hdc, int totalPages);
    bool CheckPageButtons(int mouseX, int mouseY);

    // 좌표 변환
    int GetTileIndexFromPosition(int x, int y);
    RECT GetTileRect(int index);
};