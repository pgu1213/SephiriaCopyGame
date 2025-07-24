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
    , m_ShowGrid(true)
    , m_EnableCulling(true)
    , m_RoomWidth(DEFAULT_ROOM_WIDTH)
    , m_RoomHeight(DEFAULT_ROOM_HEIGHT)
    , m_ShowRoomBounds(true)
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
    cout << "타일맵 에디터 초기화 완료 (그리드: " << GRID_SIZE << "px 고정)" << endl;
    cout << "방 크기: " << m_RoomWidth << " x " << m_RoomHeight << " 그리드" << endl;
}

void TileMapEditor::Update()
{
    HandleInput();
}

void TileMapEditor::HandleInput()
{
    auto inputMgr = InputManager::GetInstance();

    POINT currentMousePos = inputMgr->GetMousePosition();

    if (inputMgr->IsKeyDown(VK_LBUTTON))
    {
        PlaceTile();
        m_LeftMousePressed = true;
    }
    else if (inputMgr->IsKeyPressed(VK_LBUTTON) && m_LeftMousePressed)
    {
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

    if (inputMgr->IsKeyDown(VK_RBUTTON))
    {
        RemoveTile();
        m_RightMousePressed = true;
    }
    else if (inputMgr->IsKeyPressed(VK_RBUTTON) && m_RightMousePressed)
    {
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

    // 그리드 토글
    if (inputMgr->IsKeyDown('G'))
    {
        m_ShowGrid = !m_ShowGrid;
        cout << "그리드 " << (m_ShowGrid ? "표시" : "숨김") << endl;
    }

    // 방 경계 토글
    if (inputMgr->IsKeyDown('B'))
    {
        m_ShowRoomBounds = !m_ShowRoomBounds;
        cout << "방 경계 " << (m_ShowRoomBounds ? "표시" : "숨김") << endl;
    }
}

void TileMapEditor::PlaceTile()
{
    if (!m_pTilePalette) return;

    wstring selectedTile = m_pTilePalette->GetSelectedTile();
    if (selectedTile.empty()) return;

    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    // 그리드에 맞춰 정렬
    int gridX = (int)(worldX / GRID_SIZE);
    int gridY = (int)(worldY / GRID_SIZE);

    // 방 경계 체크
    if (!IsWithinRoomBounds(gridX, gridY))
    {
        return; // 방 경계를 벗어나면 배치하지 않음
    }

    // 타일 배치
    pair<int, int> gridPos = make_pair(gridX, gridY);
    m_TileMap[gridPos] = TileData(selectedTile, gridX * GRID_SIZE, gridY * GRID_SIZE);
}

void TileMapEditor::RemoveTile()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    int gridX = (int)(worldX / GRID_SIZE);
    int gridY = (int)(worldY / GRID_SIZE);

    // 방 경계 체크
    if (!IsWithinRoomBounds(gridX, gridY))
    {
        return;
    }

    pair<int, int> gridPos = make_pair(gridX, gridY);
    m_TileMap.erase(gridPos);
}

void TileMapEditor::Render(HDC hdc)
{
    if (m_ShowGrid)
    {
        RenderGrid(hdc);
    }

    if (m_ShowRoomBounds)
    {
        RenderRoomBounds(hdc);
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

    // 방 경계 내에서만 그리드 계산
    float worldLeft = 0;
    float worldTop = 0;
    float worldRight = m_RoomWidth * GRID_SIZE;
    float worldBottom = m_RoomHeight * GRID_SIZE;

    // 수직선 (방 경계 내에서만)
    for (int x = 0; x <= m_RoomWidth; x++)
    {
        float worldX = x * GRID_SIZE;
        POINT start = m_pCamera->WorldToScreen(worldX, worldTop);
        POINT end = m_pCamera->WorldToScreen(worldX, worldBottom);
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, end.x, end.y);
    }

    // 수평선 (방 경계 내에서만)
    for (int y = 0; y <= m_RoomHeight; y++)
    {
        float worldY = y * GRID_SIZE;
        POINT start = m_pCamera->WorldToScreen(worldLeft, worldY);
        POINT end = m_pCamera->WorldToScreen(worldRight, worldY);
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, end.x, end.y);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
}

void TileMapEditor::RenderTiles(HDC hdc)
{
    Graphics graphics(hdc);
    float zoom = m_pCamera->GetZoom();
    int scaledGridSize = (int)(GRID_SIZE * zoom); // 줌에 따른 스프라이트 크기

    for (const auto& tilePair : m_TileMap)
    {
        const TileData& tile = tilePair.second;
        if (tile.isEmpty) continue;

        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(tile.tileName);
        if (!tileSprite) continue;

        POINT screenPos = m_pCamera->WorldToScreen((float)tile.x, (float)tile.y);

        // 줌에 따라 스프라이트 크기 조절
        graphics.DrawImage(tileSprite, screenPos.x, screenPos.y, scaledGridSize, scaledGridSize);
    }
}

void TileMapEditor::RenderCulledTiles(HDC hdc)
{
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    float worldLeft, worldTop, worldRight, worldBottom;
    m_pCamera->ScreenToWorld(0, 0, worldLeft, worldTop);
    m_pCamera->ScreenToWorld(clientRect.right, clientRect.bottom, worldRight, worldBottom);

    // 여백 추가
    int margin = GRID_SIZE * 2;
    worldLeft -= margin;
    worldTop -= margin;
    worldRight += margin;
    worldBottom += margin;

    Graphics graphics(hdc);
    float zoom = m_pCamera->GetZoom();
    int scaledGridSize = (int)(GRID_SIZE * zoom); // 줌에 따른 스프라이트 크기
    int renderedCount = 0;

    for (const auto& tilePair : m_TileMap)
    {
        const TileData& tile = tilePair.second;
        if (tile.isEmpty) continue;

        // 컬링 검사
        if (tile.x + GRID_SIZE < worldLeft || tile.x > worldRight ||
            tile.y + GRID_SIZE < worldTop || tile.y > worldBottom)
        {
            continue;
        }

        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(tile.tileName);
        if (!tileSprite) continue;

        POINT screenPos = m_pCamera->WorldToScreen((float)tile.x, (float)tile.y);

        // 줌에 따라 스프라이트 크기 조절
        graphics.DrawImage(tileSprite, screenPos.x, screenPos.y, scaledGridSize, scaledGridSize);
        renderedCount++;
    }

    // 렌더링 통계
    SetTextColor(hdc, RGB(255, 255, 0));
    SetBkMode(hdc, TRANSPARENT);
    wstring stats = L"Rendered: " + to_wstring(renderedCount) + L"/" + to_wstring(m_TileMap.size());
    TextOut(hdc, 10, 70, stats.c_str(), stats.length());
}

void TileMapEditor::RenderRoomBounds(HDC hdc)
{
    HPEN boundsPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0)); // 빨간색 테두리
    HPEN oldPen = (HPEN)SelectObject(hdc, boundsPen);

    // 방 경계 계산 (월드 좌표)
    float worldLeft = 0;
    float worldTop = 0;
    float worldRight = m_RoomWidth * GRID_SIZE;
    float worldBottom = m_RoomHeight * GRID_SIZE;

    // 화면 좌표로 변환
    POINT topLeft = m_pCamera->WorldToScreen(worldLeft, worldTop);
    POINT topRight = m_pCamera->WorldToScreen(worldRight, worldTop);
    POINT bottomLeft = m_pCamera->WorldToScreen(worldLeft, worldBottom);
    POINT bottomRight = m_pCamera->WorldToScreen(worldRight, worldBottom);

    // 방 경계 그리기
    MoveToEx(hdc, topLeft.x, topLeft.y, NULL);
    LineTo(hdc, topRight.x, topRight.y);
    LineTo(hdc, bottomRight.x, bottomRight.y);
    LineTo(hdc, bottomLeft.x, bottomLeft.y);
    LineTo(hdc, topLeft.x, topLeft.y);

    SelectObject(hdc, oldPen);
    DeleteObject(boundsPen);
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
        wcout << L"파일 저장 실패: " << filename << endl;
        return;
    }

    // 헤더 정보
    file << L"# Tile Map Data" << endl;
    file << L"# Format: TileName X Y" << endl;
    file << L"TileCount=" << m_TileMap.size() << endl;
    file << endl;

    // 타일 데이터 (파일명 기반으로 저장)
    for (const auto& tilePair : m_TileMap)
    {
        const TileData& tile = tilePair.second;
        if (!tile.isEmpty)
        {
            file << tile.tileName << L" " << tilePair.first.first << L" " << tilePair.first.second << endl;
        }
    }

    file.close();
    wcout << L"맵 저장 완료: " << filename << L" (타일 " << m_TileMap.size() << L"개)" << endl;
}

void TileMapEditor::LoadMap()
{
    // 가장 최근 맵 파일 찾기
    wstring filename = L"Maps/latest_map.txt";
    LoadMapWithTileNames(filename);
}

void TileMapEditor::SetRoomBounds(int width, int height)
{
    m_RoomWidth = max(10, min(100, width));   // 10~100 그리드로 제한
    m_RoomHeight = max(10, min(100, height)); // 10~100 그리드로 제한
    cout << "방 크기 설정: " << m_RoomWidth << " x " << m_RoomHeight << " 그리드" << endl;
}

bool TileMapEditor::IsWithinRoomBounds(int gridX, int gridY) const
{
    return (gridX >= 0 && gridX < m_RoomWidth && gridY >= 0 && gridY < m_RoomHeight);
}

void TileMapEditor::LoadMapWithTileNames(const wstring& filename)
{
    wifstream file(filename);
    if (!file.is_open())
    {
        wcout << L"파일 로드 실패: " << filename << endl;
        return;
    }

    m_TileMap.clear();
    wstring line;
    int loadedTiles = 0;

    while (getline(file, line))
    {
        // 주석이나 빈 줄 건너뛰기
        if (line.empty() || line[0] == L'#') continue;

        if (line.find(L"TileCount=") == 0) continue;

        // 타일 데이터 파싱
        wistringstream iss(line);
        wstring tileName;
        int gridX, gridY;

        if (iss >> tileName >> gridX >> gridY)
        {
            pair<int, int> gridPos = make_pair(gridX, gridY);
            m_TileMap[gridPos] = TileData(tileName, gridX, gridY);
            loadedTiles++;
        }
    }

    file.close();
    wcout << L"맵 로드 완료: " << filename << L" (타일 " << loadedTiles << L"개)" << endl;
}

wstring TileMapEditor::GenerateMapFileName()
{
    // Maps 디렉토리 생성
    filesystem::create_directories(L"Maps");

    // 현재 시간 기반 파일명 생성
    time_t now = time(0);
    tm* timeinfo = localtime(&now);

    wchar_t buffer[256];
    wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"Maps/map_%Y%m%d_%H%M%S.txt", timeinfo);

    // latest_map.txt로도 복사 저장
    wstring latestPath = L"Maps/latest_map.txt";

    return wstring(buffer);
}

void TileMapEditor::Release()
{
    m_TileMap.clear();
}