#include "../../pch.h"
#include "TileMapEditor.h"
#include "../Camera/Camera.h"
#include "../TilePalette/TilePalette.h"
#include "../../Managers/InputManager/InputManager.h"
#include "../../Managers/ResourceManager/ResourceManager.h"
#include "../../Managers/FileManager/FileManager.h"
#include <fstream>
#include <sstream>

TileMapEditor::TileMapEditor()
    : m_pCamera(nullptr)
    , m_pTilePalette(nullptr)
    , m_GridSize(32)
    , m_ShowGrid(true)
    , m_EnableCulling(true)
    , m_LeftMousePressed(false)
    , m_RightMousePressed(false)
{
    m_LastMousePos = { 0, 0 };
}

TileMapEditor::~TileMapEditor()
{
    Release();
}

void TileMapEditor::Initialize(Camera* camera)
{
    m_pCamera = camera;
    cout << "Ÿ�ϸ� ������ �ʱ�ȭ �Ϸ�" << endl;
}

void TileMapEditor::Update()
{
    HandleInput();
}

void TileMapEditor::HandleInput()
{
    auto inputMgr = InputManager::GetInstance();

    // ���콺 ��ġ ���� (InputManager���� �̹� Ŭ���̾�Ʈ ��ǥ�� ��ȯ��)
    POINT currentMousePos = inputMgr->GetMousePosition();

    // ���콺 ���� ��ư - Ÿ�� ��ġ
    if (inputMgr->IsKeyDown(VK_LBUTTON))
    {
        PlaceTile();
        m_LeftMousePressed = true;
    }
    else if (inputMgr->IsKeyPressed(VK_LBUTTON) && m_LeftMousePressed)
    {
        // �巡�� ���� ���� Ÿ�� ��ġ
        if (abs(currentMousePos.x - m_LastMousePos.x) > 5 ||
            abs(currentMousePos.y - m_LastMousePos.y) > 5)
        {
            PlaceTile();
        }
    }
    else if (inputMgr->IsKeyUp(VK_LBUTTON))
    {
        m_LeftMousePressed = false;
    }

    // ���콺 ������ ��ư - Ÿ�� ����
    if (inputMgr->IsKeyDown(VK_RBUTTON))
    {
        RemoveTile();
        m_RightMousePressed = true;
    }
    else if (inputMgr->IsKeyPressed(VK_RBUTTON) && m_RightMousePressed)
    {
        // �巡�� ���� ���� Ÿ�� ����
        if (abs(currentMousePos.x - m_LastMousePos.x) > 5 ||
            abs(currentMousePos.y - m_LastMousePos.y) > 5)
        {
            RemoveTile();
        }
    }
    else if (inputMgr->IsKeyUp(VK_RBUTTON))
    {
        m_RightMousePressed = false;
    }

    m_LastMousePos = currentMousePos;

    // �׸��� ���
    if (inputMgr->IsKeyDown('G'))
    {
        m_ShowGrid = !m_ShowGrid;
        cout << "�׸��� " << (m_ShowGrid ? "ǥ��" : "����") << endl;
    }
}

void TileMapEditor::PlaceTile()
{
    // Ÿ�� �ȷ�Ʈ���� ���õ� Ÿ�� ��������
    if (!m_pTilePalette) return;

    wstring selectedTile = m_pTilePalette->GetSelectedTile();
    if (selectedTile.empty()) return;

    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    // �׸��忡 ���� ����
    int gridX = (int)(worldX / m_GridSize);
    int gridY = (int)(worldY / m_GridSize);

    // Ÿ�� ��ġ
    pair<int, int> gridPos = make_pair(gridX, gridY);
    m_TileMap[gridPos] = TileData(selectedTile, gridX * m_GridSize, gridY * m_GridSize);
}

void TileMapEditor::RemoveTile()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    // �׸��� ��ġ ���
    int gridX = (int)(worldX / m_GridSize);
    int gridY = (int)(worldY / m_GridSize);

    // Ÿ�� ����
    pair<int, int> gridPos = make_pair(gridX, gridY);
    m_TileMap.erase(gridPos);
}

void TileMapEditor::Render(HDC hdc)
{
    if (m_ShowGrid)
    {
        RenderGrid(hdc);
    }

    if (m_EnableCulling)
    {
        RenderCulledTiles(hdc);
    }
    else
    {
        RenderTiles(hdc);
    }
}

void TileMapEditor::RenderGrid(HDC hdc)
{
    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

    // ȭ�� ���� ���
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    float worldLeft, worldTop, worldRight, worldBottom;
    m_pCamera->ScreenToWorld(0, 0, worldLeft, worldTop);
    m_pCamera->ScreenToWorld(clientRect.right, clientRect.bottom, worldRight, worldBottom);

    // �׸��� ���� �׸���
    int startX = ((int)worldLeft / m_GridSize) * m_GridSize;
    int startY = ((int)worldTop / m_GridSize) * m_GridSize;
    int endX = ((int)worldRight / m_GridSize + 1) * m_GridSize;
    int endY = ((int)worldBottom / m_GridSize + 1) * m_GridSize;

    // ������
    for (int x = startX; x <= endX; x += m_GridSize)
    {
        POINT start = m_pCamera->WorldToScreen((float)x, worldTop);
        POINT end = m_pCamera->WorldToScreen((float)x, worldBottom);
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, end.x, end.y);
    }

    // ����
    for (int y = startY; y <= endY; y += m_GridSize)
    {
        POINT start = m_pCamera->WorldToScreen(worldLeft, (float)y);
        POINT end = m_pCamera->WorldToScreen(worldRight, (float)y);
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, end.x, end.y);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
}

void TileMapEditor::RenderTiles(HDC hdc)
{
    Graphics graphics(hdc);

    for (const auto& tilePair : m_TileMap)
    {
        const TileData& tile = tilePair.second;
        if (tile.isEmpty) continue;

        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(tile.tileName);
        if (!tileSprite) continue;

        POINT screenPos = m_pCamera->WorldToScreen((float)tile.x, (float)tile.y);
        graphics.DrawImage(tileSprite, screenPos.x, screenPos.y, m_GridSize, m_GridSize);
    }
}

void TileMapEditor::RenderCulledTiles(HDC hdc)
{
    // �ø� ����ȭ: ȭ�鿡 ���̴� Ÿ�ϸ� ������
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    float worldLeft, worldTop, worldRight, worldBottom;
    m_pCamera->ScreenToWorld(0, 0, worldLeft, worldTop);
    m_pCamera->ScreenToWorld(clientRect.right, clientRect.bottom, worldRight, worldBottom);

    // ���� �߰� (ȭ�� ��� ��ó�� Ÿ�ϵ� ������)
    int margin = m_GridSize * 2;
    worldLeft -= margin;
    worldTop -= margin;
    worldRight += margin;
    worldBottom += margin;

    Graphics graphics(hdc);
    int renderedCount = 0;

    for (const auto& tilePair : m_TileMap)
    {
        const TileData& tile = tilePair.second;
        if (tile.isEmpty) continue;

        // �ø� �˻�
        if (tile.x + m_GridSize < worldLeft || tile.x > worldRight ||
            tile.y + m_GridSize < worldTop || tile.y > worldBottom)
        {
            continue; // ȭ�� ���� Ÿ���� �ǳʶٱ�
        }

        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(tile.tileName);
        if (!tileSprite) continue;

        POINT screenPos = m_pCamera->WorldToScreen((float)tile.x, (float)tile.y);
        graphics.DrawImage(tileSprite, screenPos.x, screenPos.y, m_GridSize, m_GridSize);
        renderedCount++;
    }

    // ������ ��� (����׿�)
    SetTextColor(hdc, RGB(255, 255, 0));
    SetBkMode(hdc, TRANSPARENT);
    wstring stats = L"Rendered: " + to_wstring(renderedCount) + L"/" + to_wstring(m_TileMap.size());
    TextOut(hdc, 10, 50, stats.c_str(), stats.length());
}

void TileMapEditor::SaveMap()
{
    wstring filename = GenerateMapFileName();
    SaveMapWithTileNames(filename);
}

void TileMapEditor::SaveMapWithTileNames(const wstring& filename)
{
    wofstream file(filename);
    if (!file.is_open())
    {
        wcout << L"���� ���� ����: " << filename << endl;
        return;
    }

    // ��� ����
    file << L"# Tile Map Data" << endl;
    file << L"# Format: TileName X Y" << endl;
    file << L"GridSize=" << m_GridSize << endl;
    file << L"TileCount=" << m_TileMap.size() << endl;
    file << endl;

    // Ÿ�� ������ (���ϸ� ������� ����)
    for (const auto& tilePair : m_TileMap)
    {
        const TileData& tile = tilePair.second;
        if (!tile.isEmpty)
        {
            file << tile.tileName << L" " << tilePair.first.first << L" " << tilePair.first.second << endl;
        }
    }

    file.close();
    wcout << L"�� ���� �Ϸ�: " << filename << L" (Ÿ�� " << m_TileMap.size() << L"��)" << endl;
}

void TileMapEditor::LoadMap()
{
    // ���� �ֱ� �� ���� ã��
    wstring filename = L"Maps/latest_map.txt";
    LoadMapWithTileNames(filename);
}

void TileMapEditor::LoadMapWithTileNames(const wstring& filename)
{
    wifstream file(filename);
    if (!file.is_open())
    {
        wcout << L"���� �ε� ����: " << filename << endl;
        return;
    }

    m_TileMap.clear();
    wstring line;
    int loadedTiles = 0;

    while (getline(file, line))
    {
        // �ּ��̳� �� �� �ǳʶٱ�
        if (line.empty() || line[0] == L'#') continue;

        // ���� ���ε�
        if (line.find(L"GridSize=") == 0)
        {
            m_GridSize = stoi(line.substr(9));
            continue;
        }
        if (line.find(L"TileCount=") == 0) continue;

        // Ÿ�� ������ �Ľ�
        wistringstream iss(line);
        wstring tileName;
        int gridX, gridY;

        if (iss >> tileName >> gridX >> gridY)
        {
            pair<int, int> gridPos = make_pair(gridX, gridY);
            m_TileMap[gridPos] = TileData(tileName, gridX * m_GridSize, gridY * m_GridSize);
            loadedTiles++;
        }
    }

    file.close();
    wcout << L"�� �ε� �Ϸ�: " << filename << L" (Ÿ�� " << loadedTiles << L"��)" << endl;
}

wstring TileMapEditor::GenerateMapFileName()
{
    // Maps ���丮 ����
    filesystem::create_directories(L"Maps");

    // ���� �ð� ��� ���ϸ� ����
    time_t now = time(0);
    tm* timeinfo = localtime(&now);

    wchar_t buffer[256];
    wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"Maps/map_%Y%m%d_%H%M%S.txt", timeinfo);

    // latest_map.txt�ε� ���� ����
    wstring latestPath = L"Maps/latest_map.txt";

    return wstring(buffer);
}

void TileMapEditor::Release()
{
    m_TileMap.clear();
}