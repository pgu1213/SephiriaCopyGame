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
    cout << "타일맵 에디터 초기화 완료" << endl;
}

void TileMapEditor::Update()
{
    HandleInput();
}

void TileMapEditor::HandleInput()
{
    auto inputMgr = InputManager::GetInstance();

    // 마우스 위치 추적 (InputManager에서 이미 클라이언트 좌표로 변환됨)
    POINT currentMousePos = inputMgr->GetMousePosition();

    // 마우스 왼쪽 버튼 - 타일 배치
    if (inputMgr->IsKeyDown(VK_LBUTTON))
    {
        PlaceTile();
        m_LeftMousePressed = true;
    }
    else if (inputMgr->IsKeyPressed(VK_LBUTTON) && m_LeftMousePressed)
    {
        // 드래그 중일 때도 타일 배치
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

    // 마우스 오른쪽 버튼 - 타일 제거
    if (inputMgr->IsKeyDown(VK_RBUTTON))
    {
        RemoveTile();
        m_RightMousePressed = true;
    }
    else if (inputMgr->IsKeyPressed(VK_RBUTTON) && m_RightMousePressed)
    {
        // 드래그 중일 때도 타일 제거
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
}

void TileMapEditor::PlaceTile()
{
    // 타일 팔레트에서 선택된 타일 가져오기
    if (!m_pTilePalette) return;

    wstring selectedTile = m_pTilePalette->GetSelectedTile();
    if (selectedTile.empty()) return;

    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    // 그리드에 맞춰 정렬
    int gridX = (int)(worldX / m_GridSize);
    int gridY = (int)(worldY / m_GridSize);

    // 타일 배치
    pair<int, int> gridPos = make_pair(gridX, gridY);
    m_TileMap[gridPos] = TileData(selectedTile, gridX * m_GridSize, gridY * m_GridSize);
}

void TileMapEditor::RemoveTile()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    float worldX, worldY;
    m_pCamera->ScreenToWorld(mousePos.x, mousePos.y, worldX, worldY);

    // 그리드 위치 계산
    int gridX = (int)(worldX / m_GridSize);
    int gridY = (int)(worldY / m_GridSize);

    // 타일 제거
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

    // 화면 영역 계산
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    float worldLeft, worldTop, worldRight, worldBottom;
    m_pCamera->ScreenToWorld(0, 0, worldLeft, worldTop);
    m_pCamera->ScreenToWorld(clientRect.right, clientRect.bottom, worldRight, worldBottom);

    // 그리드 라인 그리기
    int startX = ((int)worldLeft / m_GridSize) * m_GridSize;
    int startY = ((int)worldTop / m_GridSize) * m_GridSize;
    int endX = ((int)worldRight / m_GridSize + 1) * m_GridSize;
    int endY = ((int)worldBottom / m_GridSize + 1) * m_GridSize;

    // 수직선
    for (int x = startX; x <= endX; x += m_GridSize)
    {
        POINT start = m_pCamera->WorldToScreen((float)x, worldTop);
        POINT end = m_pCamera->WorldToScreen((float)x, worldBottom);
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, end.x, end.y);
    }

    // 수평선
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
    // 컬링 최적화: 화면에 보이는 타일만 렌더링
    RECT clientRect;
    GetClientRect(GetActiveWindow(), &clientRect);

    float worldLeft, worldTop, worldRight, worldBottom;
    m_pCamera->ScreenToWorld(0, 0, worldLeft, worldTop);
    m_pCamera->ScreenToWorld(clientRect.right, clientRect.bottom, worldRight, worldBottom);

    // 여백 추가 (화면 경계 근처의 타일도 렌더링)
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

        // 컬링 검사
        if (tile.x + m_GridSize < worldLeft || tile.x > worldRight ||
            tile.y + m_GridSize < worldTop || tile.y > worldBottom)
        {
            continue; // 화면 밖의 타일은 건너뛰기
        }

        Bitmap* tileSprite = ResourceManager::GetInstance()->GetSprite(tile.tileName);
        if (!tileSprite) continue;

        POINT screenPos = m_pCamera->WorldToScreen((float)tile.x, (float)tile.y);
        graphics.DrawImage(tileSprite, screenPos.x, screenPos.y, m_GridSize, m_GridSize);
        renderedCount++;
    }

    // 렌더링 통계 (디버그용)
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
        wcout << L"파일 저장 실패: " << filename << endl;
        return;
    }

    // 헤더 정보
    file << L"# Tile Map Data" << endl;
    file << L"# Format: TileName X Y" << endl;
    file << L"GridSize=" << m_GridSize << endl;
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

        // 설정 라인들
        if (line.find(L"GridSize=") == 0)
        {
            m_GridSize = stoi(line.substr(9));
            continue;
        }
        if (line.find(L"TileCount=") == 0) continue;

        // 타일 데이터 파싱
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