#pragma once

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

    // �� Ÿ�� ���� �Լ�
    void SetRoomType(RoomType type);
    RoomType GetRoomType() const { return m_RoomType; }
    wstring GetRoomTypeName() const;
    int GetRoomTypeID() const { return (int)m_RoomType; }

    // ���̾� ���� �Լ�
    void SetCurrentLayer(LayerType layer);
    LayerType GetCurrentLayer() const { return m_CurrentLayer; }
    wstring GetLayerName() const;
    void ToggleLayerVisibility(LayerType layer);
    bool IsLayerVisible(LayerType layer) const;

private:
    void HandleInput();
    void PlaceTile();
    void PlaceColorBox();
    void RemoveTile();
    void RemoveColorBox();
    void RenderGrid(HDC hdc);
    void RenderAllLayers(HDC hdc);
    void RenderTileLayer(HDC hdc, const map<pair<int, int>, TileData>& layer);
    void RenderColorLayer(HDC hdc, const map<pair<int, int>, bool>& layer, COLORREF color);
    void RenderTiles(HDC hdc);
    void RenderCulledTiles(HDC hdc);
    void RenderRoomBounds(HDC hdc); // �� ��� ǥ��
    void RenderRoomInfo(HDC hdc);
    void RenderLayerInfo(HDC hdc);

    void SaveMapWithLayers(const wstring& filename);
    void LoadMapWithLayers(const wstring& filename);
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

    // �� Ÿ�� �ý���
    RoomType m_RoomType;

	// ���̾� ������
    LayerData m_LayerData;

    // ���̾� �ý���
    LayerType m_CurrentLayer;
    map<LayerType, bool> m_LayerVisibility; // ���̾ ǥ��/����

    // ���콺 ����
    bool m_LeftMousePressed;
    bool m_RightMousePressed;
    POINT m_LastMousePos;
};