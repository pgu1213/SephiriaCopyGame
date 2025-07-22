#include "../../pch.h"
#include "TileMapEditor.h"
#include "../../Managers/InputManager/InputManager.h"
#include "../../Managers/ResourceManager/ResourceManager.h"
#include "../../Managers/FileManager/FileManager.h"
#include "../Camera/Camera.h"
#include <fstream>
#include <sstream>

TileMapEditor::TileMapEditor()
    : m_CurrentLayer(LayerType::GROUND)
    , m_SelectedTileID(0)
    , m_MapWidth(50)
    , m_MapHeight(50)
    , m_TileSize(32)
    , m_ViewportX(0), m_ViewportY(0)
    , m_ViewportWidth(800), m_ViewportHeight(600)
    , m_VisibleStartX(0), m_VisibleStartY(0)
    , m_VisibleEndX(0), m_VisibleEndY(0)
    , m_NeedUpdate(true)
    , m_BatchesDirty(true)
    , m_ShowGrid(true)
    , m_ShowColliders(true)
{
}

TileMapEditor::~TileMapEditor()
{
}

void TileMapEditor::Init()
{
    // �� ũ�� �ʱ�ȭ
    SetGridSize(m_MapWidth, m_MapHeight);

    // ���ҽ� �Ŵ������� Ÿ�� ���� ����ȭ
    auto tileFileNames = ResourceManager::GetInstance()->GetTileFileNames();

    // Ÿ�� ID�� ���ϸ� ���� ����
    int tileID = 1; // 0�� �� Ÿ�Ϸ� ����
    for (const auto& fileName : tileFileNames)
    {
        m_TileIDToFileName[tileID] = fileName;
        m_FileNameToTileID[fileName] = tileID;
        tileID++;
    }

    UpdateVisibleTiles();
}

void TileMapEditor::Update()
{
    HandleInput();

    if (m_NeedUpdate)
    {
        UpdateVisibleTiles();
        m_NeedUpdate = false;
    }
}

void TileMapEditor::HandleInput()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    // ���콺 ��ġ�� Ÿ�� ��ǥ�� ��ȯ
    int tileX = (mousePos.x + m_ViewportX) / m_TileSize;
    int tileY = (mousePos.y + m_ViewportY) / m_TileSize;

    // Ÿ�� ��ġ/����
    if (InputManager::GetInstance()->IsKeyPressed(VK_LBUTTON))
    {
        PlaceTile(tileX, tileY);
    }
    else if (InputManager::GetInstance()->IsKeyPressed(VK_RBUTTON))
    {
        RemoveTile(tileX, tileY);
    }

    // ���̾� ��ȯ (1, 2, 3 Ű)
    if (InputManager::GetInstance()->IsKeyDown('1'))
    {
        m_CurrentLayer = LayerType::GROUND;
    }
    else if (InputManager::GetInstance()->IsKeyDown('2'))
    {
        m_CurrentLayer = LayerType::UPPER_GROUND;
    }
    else if (InputManager::GetInstance()->IsKeyDown('3'))
    {
        m_CurrentLayer = LayerType::COLLIDER;
    }

    // �׸���/�ݶ��̴� ǥ�� ���
    if (InputManager::GetInstance()->IsKeyDown('G'))
    {
        m_ShowGrid = !m_ShowGrid;
    }
    if (InputManager::GetInstance()->IsKeyDown('C'))
    {
        m_ShowColliders = !m_ShowColliders;
    }
}

void TileMapEditor::Render(HDC hdc)
{
    // ���� ����ȭ: ��ġ ������ ���
    RenderWithBatching(hdc);

    // �׸��� ������
    if (m_ShowGrid)
    {
        RenderGrid(hdc);
    }

    // �ݶ��̴� ���̾� ������ (���ýÿ���)
    if (m_CurrentLayer == LayerType::COLLIDER || m_ShowColliders)
    {
        RenderColliderLayer(hdc);
    }
}

void TileMapEditor::RenderVisibleTiles(HDC hdc)
{
    // Ground ���̾� ������
    for (int y = m_VisibleStartY; y <= m_VisibleEndY; ++y)
    {
        for (int x = m_VisibleStartX; x <= m_VisibleEndX; ++x)
        {
            if (IsValidPosition(x, y))
            {
                const auto& groundTile = m_GroundLayer[y][x];
                if (groundTile.tileID > 0)
                {
                    auto bitmap = ResourceManager::GetInstance()->GetTileBitmap(groundTile.fileName);
                    if (bitmap)
                    {
                        int screenX = x * m_TileSize - m_ViewportX;
                        int screenY = y * m_TileSize - m_ViewportY;
                        ResourceManager::GetInstance()->DrawBitmap(hdc, bitmap, screenX, screenY, m_TileSize, m_TileSize);
                    }
                }
            }
        }
    }

    // UpperGround ���̾� ������
    for (int y = m_VisibleStartY; y <= m_VisibleEndY; ++y)
    {
        for (int x = m_VisibleStartX; x <= m_VisibleEndX; ++x)
        {
            if (IsValidPosition(x, y))
            {
                const auto& upperTile = m_UpperGroundLayer[y][x];
                if (upperTile.tileID > 0)
                {
                    auto bitmap = ResourceManager::GetInstance()->GetTileBitmap(upperTile.fileName);
                    if (bitmap)
                    {
                        int screenX = x * m_TileSize - m_ViewportX;
                        int screenY = y * m_TileSize - m_ViewportY;
                        ResourceManager::GetInstance()->DrawBitmap(hdc, bitmap, screenX, screenY, m_TileSize, m_TileSize);
                    }
                }
            }
        }
    }
}

void TileMapEditor::RenderColliderLayer(HDC hdc)
{
    // �ݶ��̴��� �ʷϻ� ������ �ڽ��� ǥ��
    HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, greenBrush);

    // ������ ȿ���� ���� ����
    SetBkMode(hdc, TRANSPARENT);

    for (int y = m_VisibleStartY; y <= m_VisibleEndY; ++y)
    {
        for (int x = m_VisibleStartX; x <= m_VisibleEndX; ++x)
        {
            if (IsValidPosition(x, y) && m_ColliderLayer[y][x].hasCollider)
            {
                int screenX = x * m_TileSize - m_ViewportX;
                int screenY = y * m_TileSize - m_ViewportY;

                RECT rect = { screenX, screenY, screenX + m_TileSize, screenY + m_TileSize };

                // ������ �ڽ� �׸���
                FillRect(hdc, &rect, greenBrush);

                // �׵θ� �׸���
                HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(0, 200, 0));
                HPEN oldPen = (HPEN)SelectObject(hdc, greenPen);

                MoveToEx(hdc, rect.left, rect.top, NULL);
                LineTo(hdc, rect.right, rect.top);
                LineTo(hdc, rect.right, rect.bottom);
                LineTo(hdc, rect.left, rect.bottom);
                LineTo(hdc, rect.left, rect.top);

                SelectObject(hdc, oldPen);
                DeleteObject(greenPen);
            }
        }
    }

    SelectObject(hdc, oldBrush);
    DeleteObject(greenBrush);
}

void TileMapEditor::RenderGrid(HDC hdc)
{
    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

    // ���μ�
    for (int x = m_VisibleStartX; x <= m_VisibleEndX + 1; ++x)
    {
        int screenX = x * m_TileSize - m_ViewportX;
        if (screenX >= 0 && screenX <= m_ViewportWidth)
        {
            MoveToEx(hdc, screenX, 0, NULL);
            LineTo(hdc, screenX, m_ViewportHeight);
        }
    }

    // ���μ�
    for (int y = m_VisibleStartY; y <= m_VisibleEndY + 1; ++y)
    {
        int screenY = y * m_TileSize - m_ViewportY;
        if (screenY >= 0 && screenY <= m_ViewportHeight)
        {
            MoveToEx(hdc, 0, screenY, NULL);
            LineTo(hdc, m_ViewportWidth, screenY);
        }
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
}

void TileMapEditor::PlaceTile(int x, int y)
{
    if (!IsValidPosition(x, y)) return;

    bool changed = false;

    if (m_CurrentLayer == LayerType::GROUND)
    {
        if (m_GroundLayer[y][x].tileID != m_SelectedTileID)
        {
            m_GroundLayer[y][x].tileID = m_SelectedTileID;
            m_GroundLayer[y][x].fileName = GetFileNameFromTileID(m_SelectedTileID);
            changed = true;
        }
    }
    else if (m_CurrentLayer == LayerType::UPPER_GROUND)
    {
        if (m_UpperGroundLayer[y][x].tileID != m_SelectedTileID)
        {
            m_UpperGroundLayer[y][x].tileID = m_SelectedTileID;
            m_UpperGroundLayer[y][x].fileName = GetFileNameFromTileID(m_SelectedTileID);
            changed = true;
        }
    }
    else if (m_CurrentLayer == LayerType::COLLIDER)
    {
        if (!m_ColliderLayer[y][x].hasCollider)
        {
            m_ColliderLayer[y][x].hasCollider = true;
            changed = true;
        }
    }

    // ����� ��쿡�� ��Ƽ ���� �߰�
    if (changed)
    {
        InvalidateRegion(x, y, 1, 1, m_CurrentLayer);
    }
}

void TileMapEditor::RemoveTile(int x, int y)
{
    if (!IsValidPosition(x, y)) return;

    bool changed = false;

    if (m_CurrentLayer == LayerType::GROUND)
    {
        if (m_GroundLayer[y][x].tileID != 0)
        {
            m_GroundLayer[y][x].tileID = 0;
            m_GroundLayer[y][x].fileName = L"";
            changed = true;
        }
    }
    else if (m_CurrentLayer == LayerType::UPPER_GROUND)
    {
        if (m_UpperGroundLayer[y][x].tileID != 0)
        {
            m_UpperGroundLayer[y][x].tileID = 0;
            m_UpperGroundLayer[y][x].fileName = L"";
            changed = true;
        }
    }
    else if (m_CurrentLayer == LayerType::COLLIDER)
    {
        if (m_ColliderLayer[y][x].hasCollider)
        {
            m_ColliderLayer[y][x].hasCollider = false;
            changed = true;
        }
    }

    // ����� ��쿡�� ��Ƽ ���� �߰�
    if (changed)
    {
        InvalidateRegion(x, y, 1, 1, m_CurrentLayer);
    }
}

void TileMapEditor::InvalidateRegion(int x, int y, int width, int height, LayerType layer)
{
    DirtyRegion region;
    region.x = x;
    region.y = y;
    region.width = width;
    region.height = height;
    region.layer = layer;

    m_DirtyRegions.push_back(region);
    m_BatchesDirty = true;
}

void TileMapEditor::UpdateRenderBatches()
{
    if (!m_BatchesDirty) return;

    // ���� ��ġ Ŭ����
    for (int layer = 0; layer < 3; ++layer)
    {
        m_RenderBatches[layer].clear();
    }

    // Ground ���̾� ��ġ ����
    for (int y = m_VisibleStartY; y <= m_VisibleEndY; ++y)
    {
        for (int x = m_VisibleStartX; x <= m_VisibleEndX; ++x)
        {
            if (IsValidPosition(x, y) && m_GroundLayer[y][x].tileID > 0)
            {
                int tileID = m_GroundLayer[y][x].tileID;
                RECT pos = {
                    x * m_TileSize - m_ViewportX,
                    y * m_TileSize - m_ViewportY,
                    (x + 1) * m_TileSize - m_ViewportX,
                    (y + 1) * m_TileSize - m_ViewportY
                };

                auto& batch = m_RenderBatches[0][tileID];
                if (batch.bitmap == nullptr)
                {
                    batch.bitmap = ResourceManager::GetInstance()->GetTileBitmapByID(tileID);
                    batch.tileID = tileID;
                }
                batch.positions.push_back(pos);
            }
        }
    }

    // UpperGround ���̾� ��ġ ����
    for (int y = m_VisibleStartY; y <= m_VisibleEndY; ++y)
    {
        for (int x = m_VisibleStartX; x <= m_VisibleEndX; ++x)
        {
            if (IsValidPosition(x, y) && m_UpperGroundLayer[y][x].tileID > 0)
            {
                int tileID = m_UpperGroundLayer[y][x].tileID;
                RECT pos = {
                    x * m_TileSize - m_ViewportX,
                    y * m_TileSize - m_ViewportY,
                    (x + 1) * m_TileSize - m_ViewportX,
                    (y + 1) * m_TileSize - m_ViewportY
                };

                auto& batch = m_RenderBatches[1][tileID];
                if (batch.bitmap == nullptr)
                {
                    batch.bitmap = ResourceManager::GetInstance()->GetTileBitmapByID(tileID);
                    batch.tileID = tileID;
                }
                batch.positions.push_back(pos);
            }
        }
    }

    m_BatchesDirty = false;
}

void TileMapEditor::RenderWithBatching(HDC hdc)
{
    UpdateRenderBatches();

    // Ground ���̾� ��ġ ������
    for (const auto& batchPair : m_RenderBatches[0])
    {
        const RenderBatch& batch = batchPair.second;
        if (batch.bitmap)
        {
            for (const RECT& pos : batch.positions)
            {
                ResourceManager::GetInstance()->DrawBitmap(hdc, batch.bitmap,
                    pos.left, pos.top,
                    pos.right - pos.left, pos.bottom - pos.top);
            }
        }
    }

    // UpperGround ���̾� ��ġ ������
    for (const auto& batchPair : m_RenderBatches[1])
    {
        const RenderBatch& batch = batchPair.second;
        if (batch.bitmap)
        {
            for (const RECT& pos : batch.positions)
            {
                    ResourceManager::GetInstance()->DrawBitmap(hdc, batch.bitmap,
                    pos.left, pos.top,
                    pos.right - pos.left, pos.bottom - pos.top);
            }
        }
    }
}

void TileMapEditor::SetSelectedTileID(int tileID)
{
    m_SelectedTileID = tileID;
    m_SelectedTileFileName = GetFileNameFromTileID(tileID);
}

void TileMapEditor::SaveMap(const wstring& fileName)
{
    // TileMapEditor �����͸� FileManager �������� ��ȯ
    TileMapSaveData saveData;

    // �� ���� ����
    saveData.roomType = 1;
    saveData.gridWidth = m_MapWidth;
    saveData.gridHeight = m_MapHeight;
    saveData.canSpawnMonsters = true;
    saveData.doors[0] = true;  // North
    saveData.doors[1] = false; // East
    saveData.doors[2] = false; // South
    saveData.doors[3] = false; // West

    // ���̾� ������ �ʱ�ȭ
    saveData.groundLayer.resize(m_MapHeight);
    saveData.upperGroundLayer.resize(m_MapHeight);
    saveData.colliderLayer.resize(m_MapHeight);

    for (int y = 0; y < m_MapHeight; ++y)
    {
        saveData.groundLayer[y].resize(m_MapWidth, 0);
        saveData.upperGroundLayer[y].resize(m_MapWidth, 0);
        saveData.colliderLayer[y].resize(m_MapWidth, false);
    }

    // Ÿ�� ������ ����
    for (int y = 0; y < m_MapHeight; ++y)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            // Ground ���̾� (Ÿ�� ID)
            saveData.groundLayer[y][x] = m_GroundLayer[y][x].tileID;

            // UpperGround ���̾� (Ÿ�� ID)
            saveData.upperGroundLayer[y][x] = m_UpperGroundLayer[y][x].tileID;

            // Collider ���̾� (boolean)
            saveData.colliderLayer[y][x] = m_ColliderLayer[y][x].hasCollider;
        }
    }

    // FileManager�� ���� ����
    if (FileManager::GetInstance()->SaveTileMap(fileName, saveData))
    {
        cout << "Map saved successfully: ";
        wcout << fileName << endl;

        // ��� ���
        auto stats = FileManager::GetInstance()->GetMapStatistics(fileName);
        cout << "Map Statistics:" << endl;
        cout << "  Total Tiles: " << stats.totalTiles << endl;
        cout << "  Unique Tile Types: " << stats.uniqueTileCount << endl;
        cout << "  Colliders: " << stats.colliderCount << endl;
        cout << "  File Size: " << stats.fileSize << " bytes" << endl;
    }
    else
    {
        cout << "Failed to save map: ";
        wcout << fileName << endl;
    }
}

void TileMapEditor::LoadMap(const wstring& fileName)
{
    TileMapSaveData loadData;

    if (FileManager::GetInstance()->LoadTileMap(fileName, loadData))
    {
        // �� ũ�� ����
        SetGridSize(loadData.gridWidth, loadData.gridHeight);

        // Ÿ�� ������ ����
        for (int y = 0; y < m_MapHeight; ++y)
        {
            for (int x = 0; x < m_MapWidth; ++x)
            {
                // Ground ���̾� �ε�
                m_GroundLayer[y][x].tileID = loadData.groundLayer[y][x];
                m_GroundLayer[y][x].fileName = GetFileNameFromTileID(loadData.groundLayer[y][x]);

                // UpperGround ���̾� �ε�
                m_UpperGroundLayer[y][x].tileID = loadData.upperGroundLayer[y][x];
                m_UpperGroundLayer[y][x].fileName = GetFileNameFromTileID(loadData.upperGroundLayer[y][x]);

                // Collider ���̾� �ε�
                m_ColliderLayer[y][x].hasCollider = loadData.colliderLayer[y][x];
            }
        }

        // ������Ʈ �÷��� ����
        m_NeedUpdate = true;
        m_BatchesDirty = true;
        m_DirtyRegions.clear();

        cout << "Map loaded successfully: ";
        wcout << fileName << endl;

        // �ε�� �� ���� ���
        cout << "Map Info:" << endl;
        cout << "  Size: " << loadData.gridWidth << "x" << loadData.gridHeight << endl;
        cout << "  Room Type: " << loadData.roomType << endl;
        cout << "  Can Spawn Monsters: " << (loadData.canSpawnMonsters ? "Yes" : "No") << endl;

        // ��� ���
        auto stats = FileManager::GetInstance()->GetMapStatistics(fileName);
        cout << "  Total Tiles: " << stats.totalTiles << endl;
        cout << "  Colliders: " << stats.colliderCount << endl;
    }
    else
    {
        cout << "Failed to load map: ";
        wcout << fileName << endl;
    }
}

void TileMapEditor::ClearMap()
{
    for (int y = 0; y < m_MapHeight; ++y)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            m_GroundLayer[y][x] = TileInfo{ 0, L"" };
            m_UpperGroundLayer[y][x] = TileInfo{ 0, L"" };
            m_ColliderLayer[y][x] = ColliderTile{ false };
        }
    }
    m_NeedUpdate = true;
    m_BatchesDirty = true;

    // ��Ƽ ���� Ŭ����
    m_DirtyRegions.clear();
}

void TileMapEditor::SetGridSize(int width, int height)
{
    m_MapWidth = width;
    m_MapHeight = height;

    // ���̾� ũ�� ����
    m_GroundLayer.resize(height);
    m_UpperGroundLayer.resize(height);
    m_ColliderLayer.resize(height);

    for (int y = 0; y < height; ++y)
    {
        m_GroundLayer[y].resize(width, TileInfo{ 0, L"" });
        m_UpperGroundLayer[y].resize(width, TileInfo{ 0, L"" });
        m_ColliderLayer[y].resize(width, ColliderTile{ false });
    }

    m_NeedUpdate = true;
    m_BatchesDirty = true;

    // ��Ƽ ���� Ŭ����
    m_DirtyRegions.clear();
}

void TileMapEditor::SetViewport(int x, int y, int width, int height)
{
    m_ViewportX = x;
    m_ViewportY = y;
    m_ViewportWidth = width;
    m_ViewportHeight = height;
    m_NeedUpdate = true;
}

void TileMapEditor::UpdateVisibleTiles()
{
    // ȭ�鿡 ���̴� Ÿ�� ���� ��� (���� ����ȭ)
    m_VisibleStartX = max(0, m_ViewportX / m_TileSize - 1);
    m_VisibleStartY = max(0, m_ViewportY / m_TileSize - 1);
    m_VisibleEndX = min(m_MapWidth - 1, (m_ViewportX + m_ViewportWidth) / m_TileSize + 1);
    m_VisibleEndY = min(m_MapHeight - 1, (m_ViewportY + m_ViewportHeight) / m_TileSize + 1);

    // ���� ������ ����Ǹ� ��ġ ����� �ʿ�
    m_BatchesDirty = true;
}

bool TileMapEditor::IsValidPosition(int x, int y) const
{
    return x >= 0 && x < m_MapWidth && y >= 0 && y < m_MapHeight;
}

int TileMapEditor::GetTileIDFromResourceManager(const wstring& fileName)
{
    auto it = m_FileNameToTileID.find(fileName);
    return (it != m_FileNameToTileID.end()) ? it->second : 0;
}

wstring TileMapEditor::GetFileNameFromTileID(int tileID)
{
    auto it = m_TileIDToFileName.find(tileID);
    return (it != m_TileIDToFileName.end()) ? it->second : L"";
}