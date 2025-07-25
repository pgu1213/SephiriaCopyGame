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

    // 그리드 크기 관련 함수 제거/수정
    int GetGridSize() const { return GRID_SIZE; } // 상수 반환

    // 방 크기 제한 함수 추가
    void SetRoomBounds(int width, int height);
    bool IsWithinRoomBounds(int gridX, int gridY) const;

    void SetTilePalette(class TilePalette* palette) { m_pTilePalette = palette; }

    // 방 타입 관련 함수
    void SetRoomType(RoomType type);
    RoomType GetRoomType() const { return m_RoomType; }
    wstring GetRoomTypeName() const;
    int GetRoomTypeID() const { return (int)m_RoomType; }

    // 레이어 관련 함수
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
    void RenderRoomBounds(HDC hdc); // 방 경계 표시
    void RenderRoomInfo(HDC hdc);
    void RenderLayerInfo(HDC hdc);

    void SaveMapWithLayers(const wstring& filename);
    void LoadMapWithLayers(const wstring& filename);
    wstring GenerateMapFileName();

private:
    static const int GRID_SIZE = 16; // 고정 그리드 크기
    static const int DEFAULT_ROOM_WIDTH = 50;  // 기본 방 너비 (50 * 16 = 800px)
    static const int DEFAULT_ROOM_HEIGHT = 30; // 기본 방 높이 (30 * 16 = 480px)

    map<pair<int, int>, TileData> m_TileMap;
    class Camera* m_pCamera;
    class TilePalette* m_pTilePalette;

    bool m_ShowGrid;
    bool m_EnableCulling;

    // 방 크기 제한
    int m_RoomWidth;  // 그리드 단위
    int m_RoomHeight; // 그리드 단위
    bool m_ShowRoomBounds;

    // 방 타입 시스템
    RoomType m_RoomType;

	// 레이어 데이터
    LayerData m_LayerData;

    // 레이어 시스템
    LayerType m_CurrentLayer;
    map<LayerType, bool> m_LayerVisibility; // 레이어별 표시/숨김

    // 마우스 상태
    bool m_LeftMousePressed;
    bool m_RightMousePressed;
    POINT m_LastMousePos;
};