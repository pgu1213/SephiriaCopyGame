#pragma once

struct TileData
{
    wstring tileName;
    int x, y;
    bool isEmpty;

    TileData() : tileName(L""), x(0), y(0), isEmpty(true) {}
    TileData(const wstring& name, int posX, int posY)
        : tileName(name), x(posX), y(posY), isEmpty(false) {
    }
};

class TileMapEditor
{
public:
    TileMapEditor();
    ~TileMapEditor();

public:
    void Initialize(class Camera* camera);
    void Update();
    void Render(HDC hdc);
    void Release();

    void SaveMap();
    void LoadMap();
    void SetGridSize(int size) { m_GridSize = size; }
    int GetGridSize() const { return m_GridSize; }

    // 타일 팔레트 연결
    void SetTilePalette(class TilePalette* palette) { m_pTilePalette = palette; }

private:
    void HandleInput();
    void PlaceTile();
    void RemoveTile();
    void RenderGrid(HDC hdc);
    void RenderTiles(HDC hdc);
    void RenderCulledTiles(HDC hdc); // 컬링 최적화

    // 파일 처리 (이름 기반)
    void SaveMapWithTileNames(const wstring& filename);
    void LoadMapWithTileNames(const wstring& filename);
    wstring GenerateMapFileName();

private:
    map<pair<int, int>, TileData> m_TileMap;
    class Camera* m_pCamera;
    class TilePalette* m_pTilePalette;

    int m_GridSize;
    bool m_ShowGrid;
    bool m_EnableCulling;

    // 마우스 상태
    bool m_LeftMousePressed;
    bool m_RightMousePressed;
    POINT m_LastMousePos;
};