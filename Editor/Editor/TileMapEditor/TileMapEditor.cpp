#include "../../pch.h"
#include "TileMapEditor.h"
#include "../../Managers/InputManager/InputManager.h"
#include "../../Managers/ResourceManager/ResourceManager.h"

TileMapEditor::TileMapEditor()
    : m_GridWidth(50), m_GridHeight(50), m_CurrentLayer(LayerType::BACKGROUND),
    m_SelectedTileType(TileType::FLOOR), m_SelectedSpriteIndex(0),
    m_ShowGrid(true), m_ShowTileProperties(false)
{
    // ��� ���̾ ���̵��� �ʱ�ȭ
    for (int i = 0; i < static_cast<int>(LayerType::MAX_LAYERS); ++i)
    {
        m_LayerVisible[i] = true;
    }

    // Ÿ�ϸ� �ʱ�ȭ
    for (int layer = 0; layer < static_cast<int>(LayerType::MAX_LAYERS); ++layer)
    {
        for (int x = 0; x < MAX_GRID_WIDTH; ++x)
        {
            for (int y = 0; y < MAX_GRID_HEIGHT; ++y)
            {
                m_TileMap[layer][x][y] = TileData();
            }
        }
    }
}

TileMapEditor::~TileMapEditor()
{
}

void TileMapEditor::Init(int gridWidth, int gridHeight)
{
    SetGridSize(gridWidth, gridHeight);
    printf("TileMapEditor initialized with grid size: %dx%d\n", gridWidth, gridHeight);
}

void TileMapEditor::Update(Camera* camera)
{
    HandleInput(camera);
}

void TileMapEditor::HandleInput(Camera* camera)
{
    InputManager* input = InputManager::GetInstance();
    POINT mousePos = input->GetMousePosition();

    // ���콺 ��ġ�� �׸��� ��ǥ�� ��ȯ
    int gridX, gridY;
    ScreenToGrid(mousePos.x, mousePos.y, gridX, gridY, camera);

    // ��Ŭ������ �׸���
    if (input->IsKeyPressed(VK_LBUTTON))
    {
        if (gridX >= 0 && gridX < m_GridWidth && gridY >= 0 && gridY < m_GridHeight)
        {
            PaintTile(gridX, gridY, m_SelectedTileType, m_SelectedSpriteIndex);
        }
    }

    // ��Ŭ������ �����
    if (input->IsKeyPressed(VK_RBUTTON))
    {
        if (gridX >= 0 && gridX < m_GridWidth && gridY >= 0 && gridY < m_GridHeight)
        {
            EraseTile(gridX, gridY);
        }
    }

    // ���� ���콺 ��ġ�� Ÿ�� �Ӽ� ���� (�߰� ��ư)
    if (input->IsKeyDown(VK_MBUTTON))
    {
        if (gridX >= 0 && gridX < m_GridWidth && gridY >= 0 && gridY < m_GridHeight)
        {
            ShowTileProperties(gridX, gridY);
        }
    }

    // ���̾� ���� (1-4 Ű)
    if (input->IsKeyDown('1'))
    {
        SetCurrentLayer(LayerType::BACKGROUND);
        printf("Current layer: BACKGROUND\n");
    }
    if (input->IsKeyDown('2'))
    {
        SetCurrentLayer(LayerType::COLLISION);
        printf("Current layer: COLLISION\n");
    }
    if (input->IsKeyDown('3'))
    {
        SetCurrentLayer(LayerType::DECORATION);
        printf("Current layer: DECORATION\n");
    }
    if (input->IsKeyDown('4'))
    {
        SetCurrentLayer(LayerType::INTERACTION);
        printf("Current layer: INTERACTION\n");
    }

    // ���̾� ���ü� ��� (Ctrl + 1-4)
    bool ctrlPressed = input->IsKeyPressed(VK_CONTROL);
    if (ctrlPressed)
    {
        if (input->IsKeyDown('1'))
        {
            ToggleLayerVisibility(LayerType::BACKGROUND);
        }
        if (input->IsKeyDown('2'))
        {
            ToggleLayerVisibility(LayerType::COLLISION);
        }
        if (input->IsKeyDown('3'))
        {
            ToggleLayerVisibility(LayerType::DECORATION);
        }
        if (input->IsKeyDown('4'))
        {
            ToggleLayerVisibility(LayerType::INTERACTION);
        }
    }

    // �׸��� ǥ�� ��� (G Ű)
    if (input->IsKeyDown('G'))
    {
        m_ShowGrid = !m_ShowGrid;
        printf("Grid display: %s\n", m_ShowGrid ? "ON" : "OFF");
    }

    // Ÿ�� �Ӽ� ǥ�� ��� (T Ű)
    if (input->IsKeyDown('T'))
    {
        m_ShowTileProperties = !m_ShowTileProperties;
        printf("Tile properties display: %s\n", m_ShowTileProperties ? "ON" : "OFF");
    }
}

void TileMapEditor::Render(HDC hdc, Camera* camera)
{
    // ���̾�� Ÿ�� ������
    for (int layer = 0; layer < static_cast<int>(LayerType::MAX_LAYERS); ++layer)
    {
        if (!m_LayerVisible[layer]) continue;

        for (int x = 0; x < m_GridWidth; ++x)
        {
            for (int y = 0; y < m_GridHeight; ++y)
            {
                const TileData& tile = m_TileMap[layer][x][y];
                if (tile.type != TileType::EMPTY)
                {
                    DrawTile(hdc, x, y, tile, camera);
                }
            }
        }
    }

    // �׸��� �׸���
    if (m_ShowGrid)
    {
        DrawGrid(hdc, camera);
    }

    // ���̾� �ε������� �׸���
    DrawLayerIndicator(hdc);
}

void TileMapEditor::DrawGrid(HDC hdc, Camera* camera)
{
    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

    // ���μ� �׸���
    for (int x = 0; x <= m_GridWidth; ++x)
    {
        POINT startPos = GridToScreen(x, 0, camera);
        POINT endPos = GridToScreen(x, m_GridHeight, camera);

        MoveToEx(hdc, startPos.x, startPos.y, NULL);
        LineTo(hdc, endPos.x, endPos.y);
    }

    // ���μ� �׸���
    for (int y = 0; y <= m_GridHeight; ++y)
    {
        POINT startPos = GridToScreen(0, y, camera);
        POINT endPos = GridToScreen(m_GridWidth, y, camera);

        MoveToEx(hdc, startPos.x, startPos.y, NULL);
        LineTo(hdc, endPos.x, endPos.y);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
}

void TileMapEditor::DrawTile(HDC hdc, int gridX, int gridY, const TileData& tile, Camera* camera)
{
    POINT screenPos = GridToScreen(gridX, gridY, camera);
    int tileSize = static_cast<int>(TILE_SIZE * camera->GetZoom());

    // Ÿ�� Ÿ�Կ� ���� ���� ���� (��������Ʈ�� ���� ���� ��ü)
    COLORREF tileColor;
    switch (tile.type)
    {
    case TileType::FLOOR: tileColor = RGB(200, 200, 200); break;
    case TileType::WALL: tileColor = RGB(100, 50, 50); break;
    case TileType::CLIFF: tileColor = RGB(50, 50, 50); break;
    case TileType::MONSTER_SPAWN: tileColor = RGB(255, 100, 100); break;
    case TileType::MONSTER_MOVE: tileColor = RGB(100, 255, 100); break;
    case TileType::DOOR_NORTH:
    case TileType::DOOR_SOUTH:
    case TileType::DOOR_EAST:
    case TileType::DOOR_WEST: tileColor = RGB(100, 100, 255); break;
    default: tileColor = RGB(255, 255, 255); break;
    }

    HBRUSH tileBrush = CreateSolidBrush(tileColor);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, tileBrush);

    Rectangle(hdc, screenPos.x, screenPos.y,
        screenPos.x + tileSize, screenPos.y + tileSize);

    SelectObject(hdc, oldBrush);
    DeleteObject(tileBrush);

    // Ÿ�� �Ӽ� �ؽ�Ʈ ǥ��
    if (m_ShowTileProperties && tileSize > 10)
    {
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);

        char typeChar = '?';
        switch (tile.type)
        {
        case TileType::FLOOR: typeChar = 'F'; break;
        case TileType::WALL: typeChar = 'W'; break;
        case TileType::CLIFF: typeChar = 'C'; break;
        case TileType::MONSTER_SPAWN: typeChar = 'S'; break;
        case TileType::MONSTER_MOVE: typeChar = 'M'; break;
        case TileType::DOOR_NORTH: typeChar = 'N'; break;
        case TileType::DOOR_SOUTH: typeChar = 'S'; break;
        case TileType::DOOR_EAST: typeChar = 'E'; break;
        case TileType::DOOR_WEST: typeChar = 'W'; break;
        }

        TextOutA(hdc, screenPos.x + 2, screenPos.y + 2, &typeChar, 1);
    }
}

void TileMapEditor::DrawLayerIndicator(HDC hdc)
{
    const char* layerNames[] = { "BG", "COL", "DEC", "INT" };
    int currentLayerIndex = static_cast<int>(m_CurrentLayer);

    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkColor(hdc, RGB(0, 0, 0));

    char layerText[64];
    sprintf_s(layerText, "Layer: %s", layerNames[currentLayerIndex]);
    TextOutA(hdc, 10, 10, layerText, strlen(layerText));

    // ���̾� ���ü� ǥ��
    for (int i = 0; i < static_cast<int>(LayerType::MAX_LAYERS); ++i)
    {
        COLORREF color = m_LayerVisible[i] ? RGB(0, 255, 0) : RGB(255, 0, 0);
        SetTextColor(hdc, color);

        char visibilityText[16];
        sprintf_s(visibilityText, "%s:%s", layerNames[i], m_LayerVisible[i] ? "ON" : "OFF");
        TextOutA(hdc, 10, 30 + i * 15, visibilityText, strlen(visibilityText));
    }
}

void TileMapEditor::PaintTile(int gridX, int gridY, TileType tileType, int spriteIndex)
{
    if (gridX < 0 || gridX >= m_GridWidth || gridY < 0 || gridY >= m_GridHeight) return;

    int layerIndex = static_cast<int>(m_CurrentLayer);
    TileData& tile = m_TileMap[layerIndex][gridX][gridY];

    tile.type = tileType;
    tile.spriteIndex = spriteIndex;

    // Ÿ�� Ÿ�Կ� ���� �Ӽ� ����
    switch (tileType)
    {
    case TileType::WALL:
    case TileType::CLIFF:
        tile.isWalkable = false;
        tile.canSpawnMonster = false;
        break;
    case TileType::MONSTER_SPAWN:
        tile.isWalkable = true;
        tile.canSpawnMonster = true;
        break;
    case TileType::MONSTER_MOVE:
    case TileType::FLOOR:
        tile.isWalkable = true;
        tile.canSpawnMonster = false;
        break;
    default:
        tile.isWalkable = true;
        tile.canSpawnMonster = false;
        break;
    }
}

void TileMapEditor::EraseTile(int gridX, int gridY)
{
    if (gridX < 0 || gridX >= m_GridWidth || gridY < 0 || gridY >= m_GridHeight) return;

    int layerIndex = static_cast<int>(m_CurrentLayer);
    m_TileMap[layerIndex][gridX][gridY] = TileData();
}

TileData* TileMapEditor::GetTileAt(int gridX, int gridY)
{
    if (gridX < 0 || gridX >= m_GridWidth || gridY < 0 || gridY >= m_GridHeight) return nullptr;

    int layerIndex = static_cast<int>(m_CurrentLayer);
    return &m_TileMap[layerIndex][gridX][gridY];
}

TileData* TileMapEditor::GetTileAtLayer(int layer, int gridX, int gridY)
{
    if (layer < 0 || layer >= static_cast<int>(LayerType::MAX_LAYERS)) return nullptr;
    if (gridX < 0 || gridX >= m_GridWidth || gridY < 0 || gridY >= m_GridHeight) return nullptr;

    return &m_TileMap[layer][gridX][gridY];
}

void TileMapEditor::ShowTileProperties(int gridX, int gridY)
{
    TileData* tile = GetTileAt(gridX, gridY);
    if (!tile) return;

    const char* typeNames[] = {
        "EMPTY", "FLOOR", "WALL", "CLIFF", "MONSTER_SPAWN",
        "MONSTER_MOVE", "DOOR_NORTH", "DOOR_SOUTH", "DOOR_EAST", "DOOR_WEST"
    };

    printf("Tile at (%d, %d) - Type: %s, Walkable: %s, Can Spawn Monster: %s\n",
        gridX, gridY,
        typeNames[static_cast<int>(tile->type)],
        tile->isWalkable ? "Yes" : "No",
        tile->canSpawnMonster ? "Yes" : "No");
}

void TileMapEditor::SetCurrentLayer(LayerType layer)
{
    m_CurrentLayer = layer;
}

void TileMapEditor::ToggleLayerVisibility(LayerType layer)
{
    int layerIndex = static_cast<int>(layer);
    m_LayerVisible[layerIndex] = !m_LayerVisible[layerIndex];

    const char* layerNames[] = { "BACKGROUND", "COLLISION", "DECORATION", "INTERACTION" };
    printf("Layer %s visibility: %s\n",
        layerNames[layerIndex],
        m_LayerVisible[layerIndex] ? "ON" : "OFF");
}

void TileMapEditor::ScreenToGrid(int screenX, int screenY, int& gridX, int& gridY, Camera* camera)
{
    float worldX, worldY;
    camera->ScreenToWorld(screenX, screenY, worldX, worldY);

    gridX = static_cast<int>(worldX / TILE_SIZE);
    gridY = static_cast<int>(worldY / TILE_SIZE);
}

POINT TileMapEditor::GridToScreen(int gridX, int gridY, Camera* camera)
{
    float worldX = gridX * TILE_SIZE;
    float worldY = gridY * TILE_SIZE;

    return camera->WorldToScreen(worldX, worldY);
}

void TileMapEditor::SetGridSize(int width, int height)
{
    m_GridWidth = min(width, MAX_GRID_WIDTH);
    m_GridHeight = min(height, MAX_GRID_HEIGHT);
    printf("Grid size set to: %dx%d\n", m_GridWidth, m_GridHeight);
}

void TileMapEditor::SetSelectedTile(TileType type, int spriteIndex)
{
    m_SelectedTileType = type;
    m_SelectedSpriteIndex = spriteIndex;

    const char* typeNames[] = {
        "EMPTY", "FLOOR", "WALL", "CLIFF", "MONSTER_SPAWN",
        "MONSTER_MOVE", "DOOR_NORTH", "DOOR_SOUTH", "DOOR_EAST", "DOOR_WEST"
    };

    printf("Selected tile: %s (Sprite: %d)\n",
        typeNames[static_cast<int>(type)], spriteIndex);
}