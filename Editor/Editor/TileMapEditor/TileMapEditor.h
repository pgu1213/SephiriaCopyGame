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

    // �׸��� ũ�� ���� �Լ� ����/����
    int GetGridSize() const { return GRID_SIZE; } // ��� ��ȯ

    // �� ũ�� ���� �Լ� �߰�
    void SetRoomBounds(int width, int height);
    bool IsWithinRoomBounds(int gridX, int gridY) const;

    void SetTilePalette(class TilePalette* palette) { m_pTilePalette = palette; }

private:
    void HandleInput();
    void PlaceTile();
    void RemoveTile();
    void RenderGrid(HDC hdc);
    void RenderTiles(HDC hdc);
    void RenderCulledTiles(HDC hdc);
    void RenderRoomBounds(HDC hdc); // �� ��� ǥ��

    void SaveMapWithTileNames(const wstring& filename);
    void LoadMapWithTileNames(const wstring& filename);
    wstring GenerateMapFileName();

private:
    static const int GRID_SIZE = 16; // ���� �׸��� ũ��
    static const int DEFAULT_ROOM_WIDTH = 50;  // �⺻ �� �ʺ� (50 * 16 = 800px)
    static const int DEFAULT_ROOM_HEIGHT = 30; // �⺻ �� ���� (30 * 16 = 480px)

    map<pair<int, int>, TileData> m_TileMap;
    class Camera* m_pCamera;
    class TilePalette* m_pTilePalette;

    bool m_ShowGrid;
    bool m_EnableCulling;

    // �� ũ�� ����
    int m_RoomWidth;  // �׸��� ����
    int m_RoomHeight; // �׸��� ����
    bool m_ShowRoomBounds;

    // ���콺 ����
    bool m_LeftMousePressed;
    bool m_RightMousePressed;
    POINT m_LastMousePos;
};