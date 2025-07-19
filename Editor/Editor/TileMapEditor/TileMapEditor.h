#pragma once
#include "../Camera/Camera.h"

enum class TileType
{
    EMPTY = 0,
    FLOOR = 1,
    WALL = 2,
    CLIFF = 3,
    MONSTER_SPAWN = 4,
    MONSTER_MOVE = 5,
    DOOR_NORTH = 6,
    DOOR_SOUTH = 7,
    DOOR_EAST = 8,
    DOOR_WEST = 9
};

enum class LayerType
{
    BACKGROUND = 0,
    COLLISION = 1,
    DECORATION = 2,
    INTERACTION = 3,
    MAX_LAYERS = 4
};

struct TileData
{
    TileType type;
    int spriteIndex; // ResourceManager������ ��������Ʈ �ε���
    bool isWalkable;
    bool canSpawnMonster;

    TileData() : type(TileType::EMPTY), spriteIndex(-1), isWalkable(true), canSpawnMonster(false) {}
};

class TileMapEditor
{
public:
    TileMapEditor();
    ~TileMapEditor();

public:
    void Init(int gridWidth, int gridHeight);
    void Update(Camera* camera);
    void Render(HDC hdc, Camera* camera);

    // �׸��� ���
    void DrawGrid(HDC hdc, Camera* camera);
    void SetGridSize(int width, int height);

    // �׸��� ���
    void PaintTile(int gridX, int gridY, TileType tileType, int spriteIndex);
    void EraseTile(int gridX, int gridY);

    // Ÿ�� �Ӽ� Ȯ��
    TileData* GetTileAt(int gridX, int gridY);
    TileData* GetTileAtLayer(int layer, int gridX, int gridY);
    void ShowTileProperties(int gridX, int gridY);

    // ���̾� ���
    void SetCurrentLayer(LayerType layer);
    LayerType GetCurrentLayer() const { return m_CurrentLayer; }
    void ToggleLayerVisibility(LayerType layer);

    // ��ǥ ��ȯ
    void ScreenToGrid(int screenX, int screenY, int& gridX, int& gridY, Camera* camera);
    POINT GridToScreen(int gridX, int gridY, Camera* camera);

    // ���õ� Ÿ�� ����
    void SetSelectedTile(TileType type, int spriteIndex);

private:
    static const int TILE_SIZE = 16;
    static const int MAX_GRID_WIDTH = 100;
    static const int MAX_GRID_HEIGHT = 100;

    TileData m_TileMap[static_cast<int>(LayerType::MAX_LAYERS)][MAX_GRID_WIDTH][MAX_GRID_HEIGHT];
    int m_GridWidth;
    int m_GridHeight;

    LayerType m_CurrentLayer;
    bool m_LayerVisible[static_cast<int>(LayerType::MAX_LAYERS)];

    // ���� ���õ� Ÿ��
    TileType m_SelectedTileType;
    int m_SelectedSpriteIndex;

    // �׸��� ǥ�� �ɼ�
    bool m_ShowGrid;
    bool m_ShowTileProperties;

    void HandleInput(Camera* camera);
    void DrawTile(HDC hdc, int gridX, int gridY, const TileData& tile, Camera* camera);
    void DrawLayerIndicator(HDC hdc);
};